/********************************************************************************
** Form generated from reading UI file 'hedylamarrgui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HEDYLAMARRGUI_H
#define UI_HEDYLAMARRGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_HedyLamarrGui
{
public:
    QWidget *centralwidget;
    QCustomPlot *StabilizationPlot;
    QLineEdit *LE_xCurrentCentroid;
    QLabel *label_currentX;
    QLabel *label_currentY;
    QLineEdit *LE_yCurrentCentroid;
    QLabel *label_homeX;
    QLineEdit *LE_xHomeCentroid;
    QLabel *label_homeY;
    QLineEdit *LE_yHomeCentroid;
    QLabel *label_homeCentroid;
    QLabel *label_currentCentroid;
    QPushButton *PB_North;
    QTextEdit *TE_HedyResponse;
    QPushButton *PB_West;
    QPushButton *PB_South;
    QPushButton *PB_East;
    QSpinBox *SB_stepSize;
    QLabel *label_stepsize;
    QLineEdit *LE_initialNSoffset;
    QLineEdit *LE_initialEWoffset;
    QLabel *label_initialNSoffset;
    QLabel *label_initialEWoffset;
    QLabel *label_currentOffset;
    QLabel *label_currentEWoffset;
    QLabel *label_currentNSoffset;
    QLineEdit *LE_currentNSoffset;
    QLineEdit *LE_currentEWoffset;
    QLabel *label_ASAreminder;
    QTextEdit *TE_LogHedyLamarr;
    QLabel *label_arcsec;
    QPushButton *PB_setInitialOffset;
    QPushButton *PB_goToOffset;
    QLabel *label_goToOffsetNS;
    QLabel *label_goToOffsetEW;
    QLineEdit *LE_goToOffsetEW;
    QLineEdit *LE_goToOffsetNS;
    QFrame *stabilizationFrame;
    QLabel *label_deadTime;
    QLabel *label_integrationTime;
    QLabel *label_proportionalGain;
    QLabel *label_stabilizationParameters;
    QSpinBox *SB_deadTime;
    QSpinBox *SB_integrationTime;
    QDoubleSpinBox *dSB_proportionalGain;
    QLabel *label_maxMotion;
    QSpinBox *SB_maxMotion;
    QMenuBar *menubar;
    QMenu *menuHedyLamarr_GUI;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *HedyLamarrGui)
    {
        if (HedyLamarrGui->objectName().isEmpty())
            HedyLamarrGui->setObjectName(QString::fromUtf8("HedyLamarrGui"));
        HedyLamarrGui->resize(1082, 605);
        centralwidget = new QWidget(HedyLamarrGui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        StabilizationPlot = new QCustomPlot(centralwidget);
        StabilizationPlot->setObjectName(QString::fromUtf8("StabilizationPlot"));
        StabilizationPlot->setGeometry(QRect(20, 300, 1031, 251));
        LE_xCurrentCentroid = new QLineEdit(centralwidget);
        LE_xCurrentCentroid->setObjectName(QString::fromUtf8("LE_xCurrentCentroid"));
        LE_xCurrentCentroid->setGeometry(QRect(240, 180, 71, 21));
        label_currentX = new QLabel(centralwidget);
        label_currentX->setObjectName(QString::fromUtf8("label_currentX"));
        label_currentX->setGeometry(QRect(210, 180, 16, 17));
        label_currentY = new QLabel(centralwidget);
        label_currentY->setObjectName(QString::fromUtf8("label_currentY"));
        label_currentY->setGeometry(QRect(210, 210, 16, 17));
        LE_yCurrentCentroid = new QLineEdit(centralwidget);
        LE_yCurrentCentroid->setObjectName(QString::fromUtf8("LE_yCurrentCentroid"));
        LE_yCurrentCentroid->setGeometry(QRect(240, 210, 71, 25));
        label_homeX = new QLabel(centralwidget);
        label_homeX->setObjectName(QString::fromUtf8("label_homeX"));
        label_homeX->setGeometry(QRect(50, 180, 16, 17));
        LE_xHomeCentroid = new QLineEdit(centralwidget);
        LE_xHomeCentroid->setObjectName(QString::fromUtf8("LE_xHomeCentroid"));
        LE_xHomeCentroid->setGeometry(QRect(80, 180, 71, 25));
        label_homeY = new QLabel(centralwidget);
        label_homeY->setObjectName(QString::fromUtf8("label_homeY"));
        label_homeY->setGeometry(QRect(50, 210, 16, 17));
        LE_yHomeCentroid = new QLineEdit(centralwidget);
        LE_yHomeCentroid->setObjectName(QString::fromUtf8("LE_yHomeCentroid"));
        LE_yHomeCentroid->setGeometry(QRect(80, 210, 71, 25));
        label_homeCentroid = new QLabel(centralwidget);
        label_homeCentroid->setObjectName(QString::fromUtf8("label_homeCentroid"));
        label_homeCentroid->setGeometry(QRect(40, 160, 111, 17));
        label_currentCentroid = new QLabel(centralwidget);
        label_currentCentroid->setObjectName(QString::fromUtf8("label_currentCentroid"));
        label_currentCentroid->setGeometry(QRect(210, 160, 121, 17));
        PB_North = new QPushButton(centralwidget);
        PB_North->setObjectName(QString::fromUtf8("PB_North"));
        PB_North->setGeometry(QRect(450, 110, 51, 41));
        TE_HedyResponse = new QTextEdit(centralwidget);
        TE_HedyResponse->setObjectName(QString::fromUtf8("TE_HedyResponse"));
        TE_HedyResponse->setGeometry(QRect(620, 60, 421, 61));
        PB_West = new QPushButton(centralwidget);
        PB_West->setObjectName(QString::fromUtf8("PB_West"));
        PB_West->setGeometry(QRect(400, 150, 51, 41));
        PB_South = new QPushButton(centralwidget);
        PB_South->setObjectName(QString::fromUtf8("PB_South"));
        PB_South->setGeometry(QRect(450, 190, 51, 41));
        PB_East = new QPushButton(centralwidget);
        PB_East->setObjectName(QString::fromUtf8("PB_East"));
        PB_East->setGeometry(QRect(500, 150, 51, 41));
        SB_stepSize = new QSpinBox(centralwidget);
        SB_stepSize->setObjectName(QString::fromUtf8("SB_stepSize"));
        SB_stepSize->setGeometry(QRect(470, 50, 48, 26));
        label_stepsize = new QLabel(centralwidget);
        label_stepsize->setObjectName(QString::fromUtf8("label_stepsize"));
        label_stepsize->setGeometry(QRect(390, 50, 61, 20));
        LE_initialNSoffset = new QLineEdit(centralwidget);
        LE_initialNSoffset->setObjectName(QString::fromUtf8("LE_initialNSoffset"));
        LE_initialNSoffset->setGeometry(QRect(80, 70, 71, 25));
        LE_initialEWoffset = new QLineEdit(centralwidget);
        LE_initialEWoffset->setObjectName(QString::fromUtf8("LE_initialEWoffset"));
        LE_initialEWoffset->setGeometry(QRect(80, 110, 71, 25));
        label_initialNSoffset = new QLabel(centralwidget);
        label_initialNSoffset->setObjectName(QString::fromUtf8("label_initialNSoffset"));
        label_initialNSoffset->setGeometry(QRect(40, 70, 31, 17));
        label_initialEWoffset = new QLabel(centralwidget);
        label_initialEWoffset->setObjectName(QString::fromUtf8("label_initialEWoffset"));
        label_initialEWoffset->setGeometry(QRect(40, 110, 31, 17));
        label_currentOffset = new QLabel(centralwidget);
        label_currentOffset->setObjectName(QString::fromUtf8("label_currentOffset"));
        label_currentOffset->setGeometry(QRect(210, 40, 101, 17));
        label_currentEWoffset = new QLabel(centralwidget);
        label_currentEWoffset->setObjectName(QString::fromUtf8("label_currentEWoffset"));
        label_currentEWoffset->setGeometry(QRect(210, 110, 31, 17));
        label_currentNSoffset = new QLabel(centralwidget);
        label_currentNSoffset->setObjectName(QString::fromUtf8("label_currentNSoffset"));
        label_currentNSoffset->setGeometry(QRect(210, 70, 31, 17));
        LE_currentNSoffset = new QLineEdit(centralwidget);
        LE_currentNSoffset->setObjectName(QString::fromUtf8("LE_currentNSoffset"));
        LE_currentNSoffset->setGeometry(QRect(250, 70, 71, 25));
        LE_currentEWoffset = new QLineEdit(centralwidget);
        LE_currentEWoffset->setObjectName(QString::fromUtf8("LE_currentEWoffset"));
        LE_currentEWoffset->setGeometry(QRect(250, 110, 71, 25));
        label_ASAreminder = new QLabel(centralwidget);
        label_ASAreminder->setObjectName(QString::fromUtf8("label_ASAreminder"));
        label_ASAreminder->setGeometry(QRect(40, 40, 141, 17));
        TE_LogHedyLamarr = new QTextEdit(centralwidget);
        TE_LogHedyLamarr->setObjectName(QString::fromUtf8("TE_LogHedyLamarr"));
        TE_LogHedyLamarr->setGeometry(QRect(620, 130, 421, 61));
        label_arcsec = new QLabel(centralwidget);
        label_arcsec->setObjectName(QString::fromUtf8("label_arcsec"));
        label_arcsec->setGeometry(QRect(390, 70, 51, 17));
        PB_setInitialOffset = new QPushButton(centralwidget);
        PB_setInitialOffset->setObjectName(QString::fromUtf8("PB_setInitialOffset"));
        PB_setInitialOffset->setGeometry(QRect(40, 10, 131, 25));
        PB_goToOffset = new QPushButton(centralwidget);
        PB_goToOffset->setObjectName(QString::fromUtf8("PB_goToOffset"));
        PB_goToOffset->setGeometry(QRect(620, 20, 131, 25));
        label_goToOffsetNS = new QLabel(centralwidget);
        label_goToOffsetNS->setObjectName(QString::fromUtf8("label_goToOffsetNS"));
        label_goToOffsetNS->setGeometry(QRect(760, 20, 31, 17));
        label_goToOffsetEW = new QLabel(centralwidget);
        label_goToOffsetEW->setObjectName(QString::fromUtf8("label_goToOffsetEW"));
        label_goToOffsetEW->setGeometry(QRect(900, 20, 31, 17));
        LE_goToOffsetEW = new QLineEdit(centralwidget);
        LE_goToOffsetEW->setObjectName(QString::fromUtf8("LE_goToOffsetEW"));
        LE_goToOffsetEW->setGeometry(QRect(940, 20, 71, 25));
        LE_goToOffsetNS = new QLineEdit(centralwidget);
        LE_goToOffsetNS->setObjectName(QString::fromUtf8("LE_goToOffsetNS"));
        LE_goToOffsetNS->setGeometry(QRect(800, 20, 71, 25));
        stabilizationFrame = new QFrame(centralwidget);
        stabilizationFrame->setObjectName(QString::fromUtf8("stabilizationFrame"));
        stabilizationFrame->setGeometry(QRect(620, 200, 421, 91));
        stabilizationFrame->setFrameShape(QFrame::StyledPanel);
        stabilizationFrame->setFrameShadow(QFrame::Raised);
        label_deadTime = new QLabel(stabilizationFrame);
        label_deadTime->setObjectName(QString::fromUtf8("label_deadTime"));
        label_deadTime->setGeometry(QRect(60, 30, 91, 20));
        label_integrationTime = new QLabel(stabilizationFrame);
        label_integrationTime->setObjectName(QString::fromUtf8("label_integrationTime"));
        label_integrationTime->setGeometry(QRect(20, 60, 131, 20));
        label_proportionalGain = new QLabel(stabilizationFrame);
        label_proportionalGain->setObjectName(QString::fromUtf8("label_proportionalGain"));
        label_proportionalGain->setGeometry(QRect(230, 60, 121, 17));
        label_stabilizationParameters = new QLabel(stabilizationFrame);
        label_stabilizationParameters->setObjectName(QString::fromUtf8("label_stabilizationParameters"));
        label_stabilizationParameters->setGeometry(QRect(10, 10, 181, 17));
        SB_deadTime = new QSpinBox(stabilizationFrame);
        SB_deadTime->setObjectName(QString::fromUtf8("SB_deadTime"));
        SB_deadTime->setGeometry(QRect(170, 30, 48, 26));
        SB_integrationTime = new QSpinBox(stabilizationFrame);
        SB_integrationTime->setObjectName(QString::fromUtf8("SB_integrationTime"));
        SB_integrationTime->setGeometry(QRect(170, 60, 48, 26));
        dSB_proportionalGain = new QDoubleSpinBox(stabilizationFrame);
        dSB_proportionalGain->setObjectName(QString::fromUtf8("dSB_proportionalGain"));
        dSB_proportionalGain->setGeometry(QRect(360, 60, 51, 26));
        label_maxMotion = new QLabel(stabilizationFrame);
        label_maxMotion->setObjectName(QString::fromUtf8("label_maxMotion"));
        label_maxMotion->setGeometry(QRect(230, 30, 121, 20));
        SB_maxMotion = new QSpinBox(stabilizationFrame);
        SB_maxMotion->setObjectName(QString::fromUtf8("SB_maxMotion"));
        SB_maxMotion->setGeometry(QRect(360, 30, 48, 26));
        HedyLamarrGui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(HedyLamarrGui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1082, 22));
        menuHedyLamarr_GUI = new QMenu(menubar);
        menuHedyLamarr_GUI->setObjectName(QString::fromUtf8("menuHedyLamarr_GUI"));
        HedyLamarrGui->setMenuBar(menubar);
        statusbar = new QStatusBar(HedyLamarrGui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        HedyLamarrGui->setStatusBar(statusbar);

        menubar->addAction(menuHedyLamarr_GUI->menuAction());

        retranslateUi(HedyLamarrGui);

        QMetaObject::connectSlotsByName(HedyLamarrGui);
    } // setupUi

    void retranslateUi(QMainWindow *HedyLamarrGui)
    {
        HedyLamarrGui->setWindowTitle(QApplication::translate("HedyLamarrGui", "MainWindow", nullptr));
        label_currentX->setText(QApplication::translate("HedyLamarrGui", "X", nullptr));
        label_currentY->setText(QApplication::translate("HedyLamarrGui", "Y", nullptr));
        label_homeX->setText(QApplication::translate("HedyLamarrGui", "X", nullptr));
        label_homeY->setText(QApplication::translate("HedyLamarrGui", "Y", nullptr));
        label_homeCentroid->setText(QApplication::translate("HedyLamarrGui", "Home centroid", nullptr));
        label_currentCentroid->setText(QApplication::translate("HedyLamarrGui", "Current centroid", nullptr));
        PB_North->setText(QApplication::translate("HedyLamarrGui", "North", nullptr));
        PB_West->setText(QApplication::translate("HedyLamarrGui", "West", nullptr));
        PB_South->setText(QApplication::translate("HedyLamarrGui", "South", nullptr));
        PB_East->setText(QApplication::translate("HedyLamarrGui", "East", nullptr));
        label_stepsize->setText(QApplication::translate("HedyLamarrGui", "Step size ", nullptr));
        label_initialNSoffset->setText(QApplication::translate("HedyLamarrGui", "N/S", nullptr));
        label_initialEWoffset->setText(QApplication::translate("HedyLamarrGui", "E/W", nullptr));
        label_currentOffset->setText(QApplication::translate("HedyLamarrGui", "Current offset", nullptr));
        label_currentEWoffset->setText(QApplication::translate("HedyLamarrGui", "E/W", nullptr));
        label_currentNSoffset->setText(QApplication::translate("HedyLamarrGui", "N/S", nullptr));
        label_ASAreminder->setText(QApplication::translate("HedyLamarrGui", "(from ASA software)", nullptr));
        label_arcsec->setText(QApplication::translate("HedyLamarrGui", "(arcsec)", nullptr));
        PB_setInitialOffset->setText(QApplication::translate("HedyLamarrGui", "Set initial offset ", nullptr));
        PB_goToOffset->setText(QApplication::translate("HedyLamarrGui", "Go to offset", nullptr));
        label_goToOffsetNS->setText(QApplication::translate("HedyLamarrGui", "N/S", nullptr));
        label_goToOffsetEW->setText(QApplication::translate("HedyLamarrGui", "E/W", nullptr));
        LE_goToOffsetEW->setText(QApplication::translate("HedyLamarrGui", "0.0", nullptr));
        LE_goToOffsetNS->setText(QApplication::translate("HedyLamarrGui", "0.0", nullptr));
        label_deadTime->setText(QApplication::translate("HedyLamarrGui", "Dead time (s)", nullptr));
        label_integrationTime->setText(QApplication::translate("HedyLamarrGui", "Integration time (s)", nullptr));
        label_proportionalGain->setText(QApplication::translate("HedyLamarrGui", "Proportional gain", nullptr));
        label_stabilizationParameters->setText(QApplication::translate("HedyLamarrGui", "Stabilization parameters", nullptr));
        label_maxMotion->setText(QApplication::translate("HedyLamarrGui", "Max. motion (px)", nullptr));
        menuHedyLamarr_GUI->setTitle(QApplication::translate("HedyLamarrGui", "Hedy Lamarr GUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HedyLamarrGui: public Ui_HedyLamarrGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEDYLAMARRGUI_H
