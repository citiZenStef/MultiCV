/************************************************************************/
/* MultiCV software:                                                    */
/*                                                                      */
/* MainWindow.h                                                         */
/*                                                                      */
/* Stephane Franiatte <stephane.franiatte@gmail.com>                    */
/*                                                                      */
/* Adaptation from:                                                     */
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include "UI/CameraConnectDialog.h"
#include "Camera/ImageBuffer.h"
#include "Camera/SharedImageBuffer.h"
#include "UI/listefiltres.h"

#include "UI/Forms/TLDSettings.h"
#include "UI/Forms/MOGSettings.h"
#include "UI/Forms/MoGTLDSettings.h"

#include "Vision/VisionUnit.h"
#include "VisionUnit_widget.h"

#include "AI/Robot_ai.h"

#define FPS 30

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    SharedImageBuffer *SImageBuffer;

    QPixmap AIStateImg;
    QPixmap LogoImg;

    bool removeFromMapByTabIndex(QMap<int, int> &map, int tabIndex);
    bool removeFromMapByTabIndex(QMap<int, VisionUnit_widget *> &map, int tabIndex);
    bool removeFromMapByTabIndex(QMap<int, VisionUnit *> &map, int tabIndex);
    void updateMapValues(QMap<int, int> &map, int tabIndex);

    bool IsCameraOpened;
    bool IsVideoOpened;

    QStackedWidget *StackedWidget;

    QMap<int, VisionUnit*> VisionUnitsMap;
    QMap<int, VisionUnit_widget*> VisionUnitWidgetsMap;
    QMap<int, int> DeviceNumbersMap;

    VisionUnit_widget* VideoWidget;

    TLDSettings*    TLDAlgoSettings;
    MOGSettings*    MOGAlgoSettings;
    MoGTLDSettings* MOGTLDAlgoSettings;

    int IpCamCounter;

    Robot_AI AI;

protected:

private slots:

    void InitVisionUnit();

    void Close();
    void CloseByTab();
    void on_algoComboBox_currentIndexChanged(const QString &arg1);
    void on_tabWidget_currentChanged(int index);
};

#endif // MAINWINDOW_H
