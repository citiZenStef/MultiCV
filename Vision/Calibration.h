#include <vector>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Vision/AlgoVision.h"

using namespace cv;
using namespace std;

class Calibration : public AlgoVision
{
private:
    // input points
    // Points en coordonnees world
    vector< vector<Point3f> > objectPoints;
    // Position des points en pixels
    vector< vector<Point2f> > imagePoints;

    // Matrices de sortie
    Mat cameraMatrix;
    Mat distCoeffs;

    int flag;

    Mat map1, map2;
    bool mustInitUndistort;

public:
    Calibration();

    // Open the chessboard images and extract corner points
    int addChessboardPoints( const vector<string>& filelist, cv::Size & boardSize );
    // Add scene points and corresponding image points
    void addPoints( const vector<Point2f>& imageCorners, const vector<Point3f>& objectCorners );
    // Calibrate the camera
    double calibrate( Size &imageSize );
    // Set the calibration flag
    void setCalibrationFlag( bool radial8CoeffEnabled = false, bool tangentialParamEnabled = false );
    // Remove distortion in an image (after calibration)
    Mat remap( const Mat &image );

    // Getters
    Mat getCameraMatrix() { return cameraMatrix; }
    Mat getDistCoeffs()   { return distCoeffs; }

    void execute( Mat in, CvRect* pRoi = 0, bool debug=false );
	~Calibration(void);
};

