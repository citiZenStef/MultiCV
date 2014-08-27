#-------------------------------------------------
#
# Project created by QtCreator 2012-01-12T14:46:13
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

TARGET = multiCV
TEMPLATE = app

SOURCES += \
    Vision/FiltreTracking.cpp \
    Vision/Calibration.cpp \
    Vision/VisionUnit.cpp \
    Vision/Kalman.cpp \
    Vision/SlidingWinDetector.cpp \
    AI/StateMachine.cpp \
    AI/Robot_ai.cpp \
    Camera/SharedImageBuffer.cpp \
    Camera/ProcessingThread.cpp \
    Camera/ImageBuffer.cpp \
    Camera/Chameleon.cpp \
    Camera/CaptureThread.cpp \
    UI/VisionUnit_widget.cpp \
    UI/mainwindow.cpp \
    UI/main.cpp \
    UI/listefiltres.cpp \
    UI/CameraConnectDialog.cpp \
    UI/CalibrationDialog.cpp \
    Vision/cvBlob/cvtrack.cpp \
    Vision/cvBlob/cvlabel.cpp \
    Vision/cvBlob/cvcontour.cpp \
    Vision/cvBlob/cvcolor.cpp \
    Vision/cvBlob/cvblob.cpp \
    Vision/cvBlob/cvaux.cpp \
    Controller/Phidgets1062.cpp \
    Controller/Motor.cpp \
    Controller/EventHandlers.cpp \
    Controller/CommRobot.cpp \
    Vision/libopentld/tld/VarianceFilter.cpp \
    Vision/libopentld/tld/TLDUtil.cpp \
    Vision/libopentld/tld/TLD.cpp \
    Vision/libopentld/tld/NNClassifier.cpp \
    Vision/libopentld/tld/MedianFlowTracker.cpp \
    Vision/libopentld/tld/ForegroundDetector.cpp \
    Vision/libopentld/tld/EnsembleClassifier.cpp \
    Vision/libopentld/tld/DetectorCascade.cpp \
    Vision/libopentld/tld/DetectionResult.cpp \
    Vision/libopentld/tld/Clustering.cpp \
    Vision/libopentld/imacq/ImAcq.cpp \
    Vision/libopentld/mftracker/Median.cpp \
    Vision/libopentld/mftracker/Lk.cpp \
    Vision/libopentld/mftracker/FBTrack.cpp \
    Vision/libopentld/mftracker/BBPredict.cpp \
    Vision/libopentld/mftracker/BB.cpp \
    Vision/cvblobs/ComponentLabeling.cpp \
    Vision/cvblobs/BlobResult.cpp \
    Vision/cvblobs/BlobProperties.cpp \
    Vision/cvblobs/BlobOperators.cpp \
    Vision/cvblobs/BlobContour.cpp \
    Vision/cvblobs/blob.cpp \
    Vision/SentryTLD.cpp \
    UI/FrameLabel.cpp \
    Vision/Trajectory.cpp \
    UI/Forms/TLDSettings.cpp \
    UI/Forms/MOGSettings.cpp \
    UI/Forms/AlgoSettings.cpp

HEADERS  += \
    Vision/AlgoVision.h \
    Vision/FiltreTracking.h \
    Vision/Calibration.h \
    Vision/VisionUnit.h \
    Vision/Kalman.h \
    Vision/SlidingWinDetector.h \
    AI/StateMachine.h \
    AI/Robot_ai.h \
    Camera/Structures.h \
    Camera/SharedImageBuffer.h \
    Camera/ProcessingThread.h \
    Camera/ImageBuffer.h \
    Camera/Chameleon.h \
    Camera/CaptureThread.h \
    UI/VisionUnit_widget.h \
    UI/mainwindow.h \
    UI/listefiltres.h \
    UI/Config.h \
    UI/CameraConnectDialog.h \
    UI/CalibrationDialog.h \
    Vision/cvBlob/cvblob.h \
    Camera/Config.h \
    Controller/Phidgets1062.h \
    Controller/Motor.h \
    Controller/EventHandlers.h \
    Controller/CommRobot.h \
    Vision/libopentld/tld/VarianceFilter.h \
    Vision/libopentld/tld/TLDUtil.h \
    Vision/libopentld/tld/TLD.h \
    Vision/libopentld/tld/NormalizedPatch.h \
    Vision/libopentld/tld/NNClassifier.h \
    Vision/libopentld/tld/MedianFlowTracker.h \
    Vision/libopentld/tld/IntegralImage.h \
    Vision/libopentld/tld/ForegroundDetector.h \
    Vision/libopentld/tld/EnsembleClassifier.h \
    Vision/libopentld/tld/DetectorCascade.h \
    Vision/libopentld/tld/DetectionResult.h \
    Vision/libopentld/tld/Clustering.h \
    Vision/libopentld/imacq/ImAcq.h \
    Vision/libopentld/mftracker/Median.h \
    Vision/libopentld/mftracker/Lk.h \
    Vision/libopentld/mftracker/FBTrack.h \
    Vision/libopentld/mftracker/BBPredict.h \
    Vision/libopentld/mftracker/BB.h \
    Vision/cvblobs/ComponentLabeling.h \
    Vision/cvblobs/BlobResult.h \
    Vision/cvblobs/BlobProperties.h \
    Vision/cvblobs/BlobOperators.h \
    Vision/cvblobs/BlobLibraryConfiguration.h \
    Vision/cvblobs/BlobContour.h \
    Vision/cvblobs/blob.h \
    Vision/SentryTLD.h \
    UI/FrameLabel.h \
    Vision/Trajectory.h \
    UI/Forms/TLDSettings.h \
    UI/Forms/MOGSettings.h \
    UI/Forms/AlgoSettings.h

INCLUDEPATH += /usr/local/include/ \
               /usr/include/flycapture/

LIBS += `pkg-config opencv --libs` \
        /usr/lib/libflycapture.so \
        /usr/lib/libphidget21.so \
        -fopenmp

FORMS += \
    UI/Forms/VisionUnit_widget.ui \
    UI/Forms/mainwindow.ui \
    UI/Forms/listefiltres.ui \
    UI/Forms/CameraConnectDialog.ui \
    UI/Forms/CalibrationDialog.ui \
    UI/Forms/TLDSettings.ui \
    UI/Forms/MOGSettings.ui

RESOURCES += \
    UI/Images.qrc

OTHER_FILES += \
    Vision/cvblobs/CMakeLists.txt
