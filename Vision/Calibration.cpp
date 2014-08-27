#include "Calibration.h"
#include <QVector>


Calibration::Calibration() :
    flag(0),
    mustInitUndistort(true)
{
}

void Calibration::execute(Mat in, CvRect *pRoi, bool debug)
{
    const int CHESSBOARD_HEIGHT = 6;
    const int CHESSBOARD_WIDTH  = 9;

    vector<Point2f> imageCorners;
    Size boardSize( CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT );

    bool bCornersFound = findChessboardCorners( in, boardSize, imageCorners );
    drawChessboardCorners( in, boardSize, imageCorners, bCornersFound );
}

// Open chessboard images and extract corner points
int Calibration::addChessboardPoints( const vector<string>& filelist,
                                      Size& boardSize )
{
    // the points on the chessboard
    vector<Point2f> imageCorners;
    vector<Point3f> objectCorners;

    // 3D Scene Points:
    // Initialize the chessboard corners
    // in the chessboard reference frame
    // The corners are at 3D location (X,Y,Z)= (i,j,0)
    for( int i = 0; i < boardSize.height; i++ )
    {
        for( int j = 0; j < boardSize.width; j++ )
        {
            objectCorners.push_back( Point3f(i, j, 0.0f) );
        }
    }

    // 2D Image points:
    Mat image; // to contain chessboard image
    int successes = 0;
    // for all viewpoints
    for ( int i = 0; i < filelist.size(); i++ )
    {
        // Open the image
        image = imread( filelist[i], 0 );

        // Get the chessboard corners
        bool found = findChessboardCorners( image,
                                            boardSize,
                                            imageCorners );

        // Get subpixel accuracy on the corners
        cornerSubPix( image, imageCorners,
                      Size( 5, 5 ),
                      Size(-1,-1 ),
                      TermCriteria( TermCriteria::MAX_ITER + TermCriteria::EPS,
                                    30,		// max number of iterations
                                    0.1 ) );     // min accuracy

        // If we have a good board, add it to our data
        if ( imageCorners.size() == boardSize.area() )
        {
            // Add image and scene points from one view
            addPoints(imageCorners, objectCorners);
            successes++;
        }

        //Draw the corners
        drawChessboardCorners( image, boardSize, imageCorners, found );
        imshow( "Corners on Chessboard", image );
        waitKey( 100 );
    }

    return successes;
}

// Add scene points and corresponding image points
void Calibration::addPoints(const vector<Point2f>& imageCorners, const vector<Point3f>& objectCorners)
{
    // 2D image points from one view
    imagePoints.push_back( imageCorners );
    // corresponding 3D scene points
    objectPoints.push_back( objectCorners );
}

// Calibrate the camera
// returns the re-projection error
double Calibration::calibrate(Size &imageSize)
{
    // undistorter must be reinitialized
    mustInitUndistort= true;

    //Output rotations and translations
    vector<Mat> rvecs, tvecs;

    // start calibration
    return calibrateCamera( objectPoints, // the 3D points
                            imagePoints,  // the image points
                            imageSize,    // image size
                            cameraMatrix, // output camera matrix
                            distCoeffs,   // output distortion matrix
                            rvecs, tvecs, // Rs, Ts
                            flag );       // set options
//					,CV_CALIB_USE_INTRINSIC_GUESS);

}

// remove distortion in an image (after calibration)
Mat Calibration::remap( const Mat &image )
{
    Mat undistorted;

    if( mustInitUndistort )
    {
        // called once per calibration
        initUndistortRectifyMap( cameraMatrix,  // computed camera matrix
                                 distCoeffs,    // computed distortion matrix
                                 Mat(),     // optional rectification (none)
                                 Mat(),     // camera matrix to generate undistorted
                                 Size(640,480),
                                 //            image.size(),  // size of undistorted
                                 CV_32FC1,      // type of output map
                                 map1, map2 );   // the x and y mapping functions

        mustInitUndistort= false;
    }

    // Apply mapping functions
    cv::remap( image,
               undistorted,
               map1, map2,
               INTER_LINEAR ); // interpolation type

    return undistorted;
}


// Set the calibration options
// 8radialCoeffEnabled should be true if 8 radial coefficients are required (5 is default)
// tangentialParamEnabled should be true if tangeantial distortion is present
void Calibration::setCalibrationFlag(bool radial8CoeffEnabled, bool tangentialParamEnabled)
{
    // Set the flag used in calibrateCamera()
    flag = 0;
    if ( !tangentialParamEnabled ) flag += CV_CALIB_ZERO_TANGENT_DIST;
    if ( radial8CoeffEnabled ) flag += CV_CALIB_RATIONAL_MODEL;
}


Calibration::~Calibration(void)
{
}
