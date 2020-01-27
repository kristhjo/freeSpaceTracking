/********************************************************************************
** Form generated from reading UI file 'trackinggui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKINGGUI_H
#define UI_TRACKINGGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackingGui
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_Hexapod;
    QPushButton *PB_StopStabilization;
    QPushButton *PB_ConnectHexapod;
    QPushButton *PB_StartStabilization;
    QPushButton *PB_ControlHexapod;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_Seeing;
    QPushButton *PB_ConfigureSeeing;
    QPushButton *PB_StartSeeingMeasurement;
    QPushButton *PB_StopSeeingMeasurement;
    QGridLayout *gridLayout_Cam;
    QPushButton *PB_initConfig;
    QComboBox *CB_Settings;
    QPushButton *PB_StartCam;
    QPushButton *PB_Disconnect;
    QPushButton *PB_ConnectCam;
    QPushButton *PB_StopCam;
    QGridLayout *gridLayout_CamSettings;
    QSlider *HS_ExposureCam;
    QLabel *Label_Height;
    QLabel *Label_OffsetX;
    QSlider *HS_OffsetY;
    QSpinBox *SB_Width;
    QSlider *HS_OffsetX;
    QSpinBox *SB_Height;
    QSlider *HS_Width;
    QLabel *Label_OffsetY;
    QLabel *Label_ExposureCam;
    QLabel *Label_FrameRate;
    QSpinBox *SB_FrameRate;
    QLabel *Label_FrameSize;
    QSlider *HS_Height;
    QSpinBox *SB_OffsetX;
    QSpinBox *SB_ExposureCam;
    QHBoxLayout *horizontalLayout;
    QLabel *Label_Gain;
    QSpinBox *SB_GainCam;
    QSpinBox *SB_OffsetY;
    QSpacerItem *verticalSpacer;
    QGridLayout *gridLayout_Tracking;
    QPushButton *PB_StartTracking;
    QPushButton *PB_UseThresholding;
    QPushButton *PB_UseWindowing;
    QHBoxLayout *horizontalLayout_Tracking;
    QLabel *Label_WindowRadius;
    QSpinBox *SB_WindowRadius;
    QLabel *Label_TrackingThreshold;
    QSpinBox *SB_TrackingThresh;
    QPushButton *PB_StopTracking;
    QTextEdit *TE_LogCam;
    QMenuBar *menuBar;
    QMenu *menuTracking_GUI;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TrackingGui)
    {
        if (TrackingGui->objectName().isEmpty())
            TrackingGui->setObjectName(QString::fromUtf8("TrackingGui"));
        TrackingGui->resize(1268, 885);
        centralWidget = new QWidget(TrackingGui);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_Hexapod = new QGridLayout();
        gridLayout_Hexapod->setSpacing(6);
        gridLayout_Hexapod->setObjectName(QString::fromUtf8("gridLayout_Hexapod"));
        PB_StopStabilization = new QPushButton(centralWidget);
        PB_StopStabilization->setObjectName(QString::fromUtf8("PB_StopStabilization"));

        gridLayout_Hexapod->addWidget(PB_StopStabilization, 2, 1, 1, 1);

        PB_ConnectHexapod = new QPushButton(centralWidget);
        PB_ConnectHexapod->setObjectName(QString::fromUtf8("PB_ConnectHexapod"));

        gridLayout_Hexapod->addWidget(PB_ConnectHexapod, 1, 0, 1, 1);

        PB_StartStabilization = new QPushButton(centralWidget);
        PB_StartStabilization->setObjectName(QString::fromUtf8("PB_StartStabilization"));

        gridLayout_Hexapod->addWidget(PB_StartStabilization, 2, 0, 1, 1);

        PB_ControlHexapod = new QPushButton(centralWidget);
        PB_ControlHexapod->setObjectName(QString::fromUtf8("PB_ControlHexapod"));

        gridLayout_Hexapod->addWidget(PB_ControlHexapod, 1, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_Hexapod, 1, 1, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_Seeing = new QVBoxLayout();
        verticalLayout_Seeing->setSpacing(6);
        verticalLayout_Seeing->setObjectName(QString::fromUtf8("verticalLayout_Seeing"));
        PB_ConfigureSeeing = new QPushButton(centralWidget);
        PB_ConfigureSeeing->setObjectName(QString::fromUtf8("PB_ConfigureSeeing"));

        verticalLayout_Seeing->addWidget(PB_ConfigureSeeing);

        PB_StartSeeingMeasurement = new QPushButton(centralWidget);
        PB_StartSeeingMeasurement->setObjectName(QString::fromUtf8("PB_StartSeeingMeasurement"));

        verticalLayout_Seeing->addWidget(PB_StartSeeingMeasurement);

        PB_StopSeeingMeasurement = new QPushButton(centralWidget);
        PB_StopSeeingMeasurement->setObjectName(QString::fromUtf8("PB_StopSeeingMeasurement"));

        verticalLayout_Seeing->addWidget(PB_StopSeeingMeasurement);


        gridLayout->addLayout(verticalLayout_Seeing, 0, 0, 1, 1);


        gridLayout_3->addLayout(gridLayout, 2, 1, 1, 1);

        gridLayout_Cam = new QGridLayout();
        gridLayout_Cam->setSpacing(6);
        gridLayout_Cam->setObjectName(QString::fromUtf8("gridLayout_Cam"));
        PB_initConfig = new QPushButton(centralWidget);
        PB_initConfig->setObjectName(QString::fromUtf8("PB_initConfig"));

        gridLayout_Cam->addWidget(PB_initConfig, 1, 0, 1, 1);

        CB_Settings = new QComboBox(centralWidget);
        CB_Settings->addItem(QString());
        CB_Settings->addItem(QString());
        CB_Settings->setObjectName(QString::fromUtf8("CB_Settings"));

        gridLayout_Cam->addWidget(CB_Settings, 1, 1, 1, 1);

        PB_StartCam = new QPushButton(centralWidget);
        PB_StartCam->setObjectName(QString::fromUtf8("PB_StartCam"));

        gridLayout_Cam->addWidget(PB_StartCam, 3, 0, 1, 1);

        PB_Disconnect = new QPushButton(centralWidget);
        PB_Disconnect->setObjectName(QString::fromUtf8("PB_Disconnect"));

        gridLayout_Cam->addWidget(PB_Disconnect, 2, 1, 1, 1);

        PB_ConnectCam = new QPushButton(centralWidget);
        PB_ConnectCam->setObjectName(QString::fromUtf8("PB_ConnectCam"));

        gridLayout_Cam->addWidget(PB_ConnectCam, 2, 0, 1, 1);

        PB_StopCam = new QPushButton(centralWidget);
        PB_StopCam->setObjectName(QString::fromUtf8("PB_StopCam"));

        gridLayout_Cam->addWidget(PB_StopCam, 3, 1, 1, 1);

        gridLayout_CamSettings = new QGridLayout();
        gridLayout_CamSettings->setSpacing(6);
        gridLayout_CamSettings->setObjectName(QString::fromUtf8("gridLayout_CamSettings"));
        HS_ExposureCam = new QSlider(centralWidget);
        HS_ExposureCam->setObjectName(QString::fromUtf8("HS_ExposureCam"));
        HS_ExposureCam->setMinimum(15);
        HS_ExposureCam->setMaximum(1000000);
        HS_ExposureCam->setOrientation(Qt::Horizontal);

        gridLayout_CamSettings->addWidget(HS_ExposureCam, 1, 1, 1, 2);

        Label_Height = new QLabel(centralWidget);
        Label_Height->setObjectName(QString::fromUtf8("Label_Height"));

        gridLayout_CamSettings->addWidget(Label_Height, 4, 0, 1, 1);

        Label_OffsetX = new QLabel(centralWidget);
        Label_OffsetX->setObjectName(QString::fromUtf8("Label_OffsetX"));

        gridLayout_CamSettings->addWidget(Label_OffsetX, 5, 0, 1, 1);

        HS_OffsetY = new QSlider(centralWidget);
        HS_OffsetY->setObjectName(QString::fromUtf8("HS_OffsetY"));
        HS_OffsetY->setOrientation(Qt::Horizontal);

        gridLayout_CamSettings->addWidget(HS_OffsetY, 6, 1, 1, 2);

        SB_Width = new QSpinBox(centralWidget);
        SB_Width->setObjectName(QString::fromUtf8("SB_Width"));
        SB_Width->setMinimum(5);
        SB_Width->setMaximum(511);

        gridLayout_CamSettings->addWidget(SB_Width, 3, 3, 1, 1);

        HS_OffsetX = new QSlider(centralWidget);
        HS_OffsetX->setObjectName(QString::fromUtf8("HS_OffsetX"));
        HS_OffsetX->setOrientation(Qt::Horizontal);

        gridLayout_CamSettings->addWidget(HS_OffsetX, 5, 1, 1, 2);

        SB_Height = new QSpinBox(centralWidget);
        SB_Height->setObjectName(QString::fromUtf8("SB_Height"));
        SB_Height->setMinimum(5);
        SB_Height->setMaximum(541);

        gridLayout_CamSettings->addWidget(SB_Height, 4, 3, 1, 1);

        HS_Width = new QSlider(centralWidget);
        HS_Width->setObjectName(QString::fromUtf8("HS_Width"));
        HS_Width->setMinimum(5);
        HS_Width->setMaximum(511);
        HS_Width->setOrientation(Qt::Horizontal);

        gridLayout_CamSettings->addWidget(HS_Width, 3, 1, 1, 2);

        Label_OffsetY = new QLabel(centralWidget);
        Label_OffsetY->setObjectName(QString::fromUtf8("Label_OffsetY"));

        gridLayout_CamSettings->addWidget(Label_OffsetY, 6, 0, 1, 1);

        Label_ExposureCam = new QLabel(centralWidget);
        Label_ExposureCam->setObjectName(QString::fromUtf8("Label_ExposureCam"));

        gridLayout_CamSettings->addWidget(Label_ExposureCam, 1, 0, 1, 1);

        Label_FrameRate = new QLabel(centralWidget);
        Label_FrameRate->setObjectName(QString::fromUtf8("Label_FrameRate"));

        gridLayout_CamSettings->addWidget(Label_FrameRate, 2, 2, 1, 1);

        SB_FrameRate = new QSpinBox(centralWidget);
        SB_FrameRate->setObjectName(QString::fromUtf8("SB_FrameRate"));
        SB_FrameRate->setMaximum(50);

        gridLayout_CamSettings->addWidget(SB_FrameRate, 2, 3, 1, 1);

        Label_FrameSize = new QLabel(centralWidget);
        Label_FrameSize->setObjectName(QString::fromUtf8("Label_FrameSize"));

        gridLayout_CamSettings->addWidget(Label_FrameSize, 3, 0, 1, 1);

        HS_Height = new QSlider(centralWidget);
        HS_Height->setObjectName(QString::fromUtf8("HS_Height"));
        HS_Height->setMinimum(5);
        HS_Height->setMaximum(541);
        HS_Height->setOrientation(Qt::Horizontal);

        gridLayout_CamSettings->addWidget(HS_Height, 4, 1, 1, 2);

        SB_OffsetX = new QSpinBox(centralWidget);
        SB_OffsetX->setObjectName(QString::fromUtf8("SB_OffsetX"));

        gridLayout_CamSettings->addWidget(SB_OffsetX, 5, 3, 1, 1);

        SB_ExposureCam = new QSpinBox(centralWidget);
        SB_ExposureCam->setObjectName(QString::fromUtf8("SB_ExposureCam"));
        SB_ExposureCam->setMinimum(15);
        SB_ExposureCam->setMaximum(1000000);

        gridLayout_CamSettings->addWidget(SB_ExposureCam, 1, 3, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Label_Gain = new QLabel(centralWidget);
        Label_Gain->setObjectName(QString::fromUtf8("Label_Gain"));

        horizontalLayout->addWidget(Label_Gain);

        SB_GainCam = new QSpinBox(centralWidget);
        SB_GainCam->setObjectName(QString::fromUtf8("SB_GainCam"));
        SB_GainCam->setMinimum(1);
        SB_GainCam->setMaximum(8);

        horizontalLayout->addWidget(SB_GainCam);


        gridLayout_CamSettings->addLayout(horizontalLayout, 2, 0, 1, 1);

        SB_OffsetY = new QSpinBox(centralWidget);
        SB_OffsetY->setObjectName(QString::fromUtf8("SB_OffsetY"));

        gridLayout_CamSettings->addWidget(SB_OffsetY, 6, 3, 1, 1);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_CamSettings->addItem(verticalSpacer, 0, 1, 1, 1);


        gridLayout_Cam->addLayout(gridLayout_CamSettings, 5, 0, 1, 2);


        gridLayout_3->addLayout(gridLayout_Cam, 0, 0, 2, 1);

        gridLayout_Tracking = new QGridLayout();
        gridLayout_Tracking->setSpacing(6);
        gridLayout_Tracking->setObjectName(QString::fromUtf8("gridLayout_Tracking"));
        PB_StartTracking = new QPushButton(centralWidget);
        PB_StartTracking->setObjectName(QString::fromUtf8("PB_StartTracking"));

        gridLayout_Tracking->addWidget(PB_StartTracking, 0, 0, 1, 1);

        PB_UseThresholding = new QPushButton(centralWidget);
        PB_UseThresholding->setObjectName(QString::fromUtf8("PB_UseThresholding"));

        gridLayout_Tracking->addWidget(PB_UseThresholding, 1, 1, 1, 1);

        PB_UseWindowing = new QPushButton(centralWidget);
        PB_UseWindowing->setObjectName(QString::fromUtf8("PB_UseWindowing"));

        gridLayout_Tracking->addWidget(PB_UseWindowing, 1, 0, 1, 1);

        horizontalLayout_Tracking = new QHBoxLayout();
        horizontalLayout_Tracking->setSpacing(6);
        horizontalLayout_Tracking->setObjectName(QString::fromUtf8("horizontalLayout_Tracking"));
        Label_WindowRadius = new QLabel(centralWidget);
        Label_WindowRadius->setObjectName(QString::fromUtf8("Label_WindowRadius"));

        horizontalLayout_Tracking->addWidget(Label_WindowRadius);

        SB_WindowRadius = new QSpinBox(centralWidget);
        SB_WindowRadius->setObjectName(QString::fromUtf8("SB_WindowRadius"));
        SB_WindowRadius->setMinimum(1);
        SB_WindowRadius->setMaximum(50);

        horizontalLayout_Tracking->addWidget(SB_WindowRadius);

        Label_TrackingThreshold = new QLabel(centralWidget);
        Label_TrackingThreshold->setObjectName(QString::fromUtf8("Label_TrackingThreshold"));

        horizontalLayout_Tracking->addWidget(Label_TrackingThreshold);

        SB_TrackingThresh = new QSpinBox(centralWidget);
        SB_TrackingThresh->setObjectName(QString::fromUtf8("SB_TrackingThresh"));
        SB_TrackingThresh->setMinimum(1);
        SB_TrackingThresh->setMaximum(255);

        horizontalLayout_Tracking->addWidget(SB_TrackingThresh);


        gridLayout_Tracking->addLayout(horizontalLayout_Tracking, 2, 0, 1, 2);

        PB_StopTracking = new QPushButton(centralWidget);
        PB_StopTracking->setObjectName(QString::fromUtf8("PB_StopTracking"));

        gridLayout_Tracking->addWidget(PB_StopTracking, 0, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_Tracking, 2, 0, 1, 1);

        TE_LogCam = new QTextEdit(centralWidget);
        TE_LogCam->setObjectName(QString::fromUtf8("TE_LogCam"));

        gridLayout_3->addWidget(TE_LogCam, 0, 1, 1, 1);

        TrackingGui->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(TrackingGui);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1268, 39));
        menuTracking_GUI = new QMenu(menuBar);
        menuTracking_GUI->setObjectName(QString::fromUtf8("menuTracking_GUI"));
        TrackingGui->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TrackingGui);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        TrackingGui->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(TrackingGui);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        TrackingGui->setStatusBar(statusBar);

        menuBar->addAction(menuTracking_GUI->menuAction());

        retranslateUi(TrackingGui);

        QMetaObject::connectSlotsByName(TrackingGui);
    } // setupUi

    void retranslateUi(QMainWindow *TrackingGui)
    {
        TrackingGui->setWindowTitle(QApplication::translate("TrackingGui", "TrackingGui", nullptr));
        PB_StopStabilization->setText(QApplication::translate("TrackingGui", "Stop stabilization", nullptr));
        PB_ConnectHexapod->setText(QApplication::translate("TrackingGui", "Connect hexapod", nullptr));
        PB_StartStabilization->setText(QApplication::translate("TrackingGui", "Start stabilization", nullptr));
        PB_ControlHexapod->setText(QApplication::translate("TrackingGui", "Manually control hexapod", nullptr));
        PB_ConfigureSeeing->setText(QApplication::translate("TrackingGui", "Configure seeing measurement", nullptr));
        PB_StartSeeingMeasurement->setText(QApplication::translate("TrackingGui", "Start seeing measurement", nullptr));
        PB_StopSeeingMeasurement->setText(QApplication::translate("TrackingGui", "Stop seeing measurement", nullptr));
        PB_initConfig->setText(QApplication::translate("TrackingGui", "Initialize configuration", nullptr));
        CB_Settings->setItemText(0, QApplication::translate("TrackingGui", "IQOQI", nullptr));
        CB_Settings->setItemText(1, QApplication::translate("TrackingGui", "Bisamberg", nullptr));

        PB_StartCam->setText(QApplication::translate("TrackingGui", "Start", nullptr));
        PB_Disconnect->setText(QApplication::translate("TrackingGui", "Disconnect Baumer", nullptr));
        PB_ConnectCam->setText(QApplication::translate("TrackingGui", "Connect Baumer", nullptr));
        PB_StopCam->setText(QApplication::translate("TrackingGui", "Stop", nullptr));
        Label_Height->setText(QApplication::translate("TrackingGui", "Height", nullptr));
        Label_OffsetX->setText(QApplication::translate("TrackingGui", "Offset X", nullptr));
        Label_OffsetY->setText(QApplication::translate("TrackingGui", "Offset Y", nullptr));
        Label_ExposureCam->setText(QApplication::translate("TrackingGui", "Exposure time (us)", nullptr));
        Label_FrameRate->setText(QApplication::translate("TrackingGui", "Frame rate", nullptr));
        Label_FrameSize->setText(QApplication::translate("TrackingGui", "Width", nullptr));
        Label_Gain->setText(QApplication::translate("TrackingGui", "Gain", nullptr));
        PB_StartTracking->setText(QApplication::translate("TrackingGui", "Start tracking", nullptr));
        PB_UseThresholding->setText(QApplication::translate("TrackingGui", "Use Tresholding", nullptr));
        PB_UseWindowing->setText(QApplication::translate("TrackingGui", "Use Windowing", nullptr));
        Label_WindowRadius->setText(QApplication::translate("TrackingGui", "Window Radius ", nullptr));
        Label_TrackingThreshold->setText(QApplication::translate("TrackingGui", "Threshold", nullptr));
        PB_StopTracking->setText(QApplication::translate("TrackingGui", "Stop tracking", nullptr));
        menuTracking_GUI->setTitle(QApplication::translate("TrackingGui", "Tracking GUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrackingGui: public Ui_TrackingGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKINGGUI_H
