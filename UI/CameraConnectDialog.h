/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* CameraConnectDialog.h                                                */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/*                                                                      */
/* Copyright (c) 2012-2013 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef CAMERACONNECTDIALOG_H
#define CAMERACONNECTDIALOG_H

// Qt
#include <QString>
#include <QDialog>
// Local
#include "Config.h"

namespace Ui {
class CameraConnectDialog;
}

class CameraConnectDialog : public QDialog
{
    Q_OBJECT
    
    public:
        explicit CameraConnectDialog(QWidget *parent=0, bool isStreamSyncEnabled=false);
        ~CameraConnectDialog();
        void setDeviceNumber();
        void setImageBufferSize();
        int getDeviceNumber();
        QString GetIPCameraURL();
        int getImageBufferSize();
        bool getDropFrameCheckBoxState();
        int getCaptureThreadPrio();
        int getProcessingThreadPrio();
        QString getTabLabel();
        bool getEnableFrameProcessingCheckBoxState();
        bool isChameleonUsed();
        bool isIPCameraUsed();
        bool isVideoUsed();

    private:
        Ui::CameraConnectDialog *ui;
        bool UseChameleon;
        bool UseVideo;
        bool UseIPCamera;

    public slots:
        void resetToDefaults();
    private slots:
        void on_checkBox_clicked(bool checked);
        void on_videoCheckBox_clicked(bool checked);
        void on_checkBox_2_clicked(bool checked);
};

#endif // CAMERACONNECTDIALOG_H
