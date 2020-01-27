/********************************************************************************
** Form generated from reading UI file 'hexapodgui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HEXAPODGUI_H
#define UI_HEXAPODGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HexapodGui
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout_2;
    QLineEdit *LE_zMove;
    QLabel *label_U;
    QLabel *label_deg_3;
    QLabel *label_V;
    QLabel *label_mm_3;
    QLineEdit *LE_uMove;
    QLabel *label_W;
    QLabel *label_mm_2;
    QLineEdit *LE_yMove;
    QLabel *label_Z;
    QLineEdit *LE_xMove;
    QLabel *label_mm;
    QLineEdit *LE_vMove;
    QLabel *label_deg;
    QLabel *label_MoveToPos;
    QLabel *label_deg_2;
    QLabel *label_Y;
    QLineEdit *LE_wMove;
    QLabel *label_X;
    QPushButton *PB_MoveToPosition;
    QGridLayout *gridLayout_3;
    QLabel *label_mm_9;
    QLabel *label_mm_8;
    QLabel *label_V_3;
    QLabel *label_ToolCo;
    QLineEdit *LE_yToolCo;
    QLineEdit *LE_vToolCo;
    QLabel *label_W_3;
    QLineEdit *LE_uToolCo;
    QLineEdit *LE_wToolCo;
    QLabel *label_deg_9;
    QLabel *label_Z_3;
    QLineEdit *LE_zToolCo;
    QLabel *label_mm_7;
    QLabel *label_X_2;
    QLabel *label_U_3;
    QLineEdit *LE_xToolCo;
    QLabel *label_deg_8;
    QLabel *label_Y_3;
    QLabel *label_deg_7;
    QPushButton *PB_setZeroCoSys;
    QGridLayout *gridLayout_9;
    QTextEdit *TE_LivePosition;
    QGridLayout *gridLayout;
    QLineEdit *LE_vActiveCo;
    QLabel *label_20;
    QLineEdit *LE_uActiveCo;
    QLineEdit *LE_yActiveCo;
    QLabel *label_22;
    QLineEdit *LE_xActiveCo;
    QLabel *label_18;
    QLabel *label_38;
    QLabel *label_36;
    QLabel *label_21;
    QLabel *label_35;
    QLabel *label_19;
    QLineEdit *LE_zActiveCo;
    QLabel *label_17;
    QLabel *label_40;
    QLabel *label_37;
    QLabel *label_39;
    QLineEdit *LE_wActiveCo;
    QPushButton *PB_ZeroOnActiveCoSys;
    QLabel *label_23;
    QGridLayout *gridLayout_StabilizationSettings;
    QGridLayout *gridLayout_8;
    QLabel *integrationTimeLabel;
    QLabel *deadTimeLabel;
    QLabel *proportionalGainLabel;
    QLabel *label_10;
    QSpinBox *SB_integrationTime;
    QLabel *label_11;
    QLabel *label_9;
    QLabel *maxMotionLabel;
    QTextEdit *TE_LogHexapod;
    QSpinBox *SB_deadTime;
    QLabel *label_8;
    QDoubleSpinBox *SB_proportionalGain;
    QDoubleSpinBox *SB_maxMotion;
    QGridLayout *gridLayout_6;
    QLineEdit *LE_uAbsCo;
    QLabel *label_Y_2;
    QLabel *label_AbsCoord;
    QLabel *label_X_3;
    QLineEdit *LE_wAbsCo;
    QLabel *label_mm_5;
    QLabel *label_mm_4;
    QLabel *label_mm_6;
    QLineEdit *LE_zAbsCo;
    QLabel *label_W_2;
    QLabel *label_U_2;
    QLineEdit *LE_yAbsCo;
    QLineEdit *LE_vAbsCo;
    QLabel *label_deg_4;
    QLineEdit *LE_xAbsCo;
    QLabel *label_deg_6;
    QLabel *label_V_2;
    QLabel *label_Z_2;
    QLabel *label_deg_5;
    QPushButton *PB_setUserDefinedCoSys;
    QGridLayout *gridLayout_7;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_2;
    QLabel *label;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_StepSize;
    QSlider *HS_IncrementSize;
    QSpinBox *SB_StepSize;
    QLabel *label_12;
    QMenuBar *menubar;
    QMenu *menuHexapod_GUI;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *HexapodGui)
    {
        if (HexapodGui->objectName().isEmpty())
            HexapodGui->setObjectName(QString::fromUtf8("HexapodGui"));
        HexapodGui->resize(2119, 816);
        centralwidget = new QWidget(HexapodGui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_5 = new QGridLayout(centralwidget);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        LE_zMove = new QLineEdit(centralwidget);
        LE_zMove->setObjectName(QString::fromUtf8("LE_zMove"));

        gridLayout_2->addWidget(LE_zMove, 3, 1, 1, 1);

        label_U = new QLabel(centralwidget);
        label_U->setObjectName(QString::fromUtf8("label_U"));

        gridLayout_2->addWidget(label_U, 1, 3, 1, 1);

        label_deg_3 = new QLabel(centralwidget);
        label_deg_3->setObjectName(QString::fromUtf8("label_deg_3"));

        gridLayout_2->addWidget(label_deg_3, 3, 5, 1, 1);

        label_V = new QLabel(centralwidget);
        label_V->setObjectName(QString::fromUtf8("label_V"));

        gridLayout_2->addWidget(label_V, 2, 3, 1, 1);

        label_mm_3 = new QLabel(centralwidget);
        label_mm_3->setObjectName(QString::fromUtf8("label_mm_3"));

        gridLayout_2->addWidget(label_mm_3, 3, 2, 1, 1);

        LE_uMove = new QLineEdit(centralwidget);
        LE_uMove->setObjectName(QString::fromUtf8("LE_uMove"));

        gridLayout_2->addWidget(LE_uMove, 1, 4, 1, 1);

        label_W = new QLabel(centralwidget);
        label_W->setObjectName(QString::fromUtf8("label_W"));

        gridLayout_2->addWidget(label_W, 3, 3, 1, 1);

        label_mm_2 = new QLabel(centralwidget);
        label_mm_2->setObjectName(QString::fromUtf8("label_mm_2"));

        gridLayout_2->addWidget(label_mm_2, 2, 2, 1, 1);

        LE_yMove = new QLineEdit(centralwidget);
        LE_yMove->setObjectName(QString::fromUtf8("LE_yMove"));

        gridLayout_2->addWidget(LE_yMove, 2, 1, 1, 1);

        label_Z = new QLabel(centralwidget);
        label_Z->setObjectName(QString::fromUtf8("label_Z"));

        gridLayout_2->addWidget(label_Z, 3, 0, 1, 1);

        LE_xMove = new QLineEdit(centralwidget);
        LE_xMove->setObjectName(QString::fromUtf8("LE_xMove"));

        gridLayout_2->addWidget(LE_xMove, 1, 1, 1, 1);

        label_mm = new QLabel(centralwidget);
        label_mm->setObjectName(QString::fromUtf8("label_mm"));

        gridLayout_2->addWidget(label_mm, 1, 2, 1, 1);

        LE_vMove = new QLineEdit(centralwidget);
        LE_vMove->setObjectName(QString::fromUtf8("LE_vMove"));

        gridLayout_2->addWidget(LE_vMove, 2, 4, 1, 1);

        label_deg = new QLabel(centralwidget);
        label_deg->setObjectName(QString::fromUtf8("label_deg"));

        gridLayout_2->addWidget(label_deg, 1, 5, 1, 1);

        label_MoveToPos = new QLabel(centralwidget);
        label_MoveToPos->setObjectName(QString::fromUtf8("label_MoveToPos"));

        gridLayout_2->addWidget(label_MoveToPos, 0, 0, 1, 6, Qt::AlignHCenter);

        label_deg_2 = new QLabel(centralwidget);
        label_deg_2->setObjectName(QString::fromUtf8("label_deg_2"));

        gridLayout_2->addWidget(label_deg_2, 2, 5, 1, 1);

        label_Y = new QLabel(centralwidget);
        label_Y->setObjectName(QString::fromUtf8("label_Y"));

        gridLayout_2->addWidget(label_Y, 2, 0, 1, 1);

        LE_wMove = new QLineEdit(centralwidget);
        LE_wMove->setObjectName(QString::fromUtf8("LE_wMove"));

        gridLayout_2->addWidget(LE_wMove, 3, 4, 1, 1);

        label_X = new QLabel(centralwidget);
        label_X->setObjectName(QString::fromUtf8("label_X"));

        gridLayout_2->addWidget(label_X, 1, 0, 1, 1);

        PB_MoveToPosition = new QPushButton(centralwidget);
        PB_MoveToPosition->setObjectName(QString::fromUtf8("PB_MoveToPosition"));

        gridLayout_2->addWidget(PB_MoveToPosition, 4, 0, 1, 6);


        gridLayout_5->addLayout(gridLayout_2, 1, 3, 1, 2);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_mm_9 = new QLabel(centralwidget);
        label_mm_9->setObjectName(QString::fromUtf8("label_mm_9"));

        gridLayout_3->addWidget(label_mm_9, 2, 2, 1, 1);

        label_mm_8 = new QLabel(centralwidget);
        label_mm_8->setObjectName(QString::fromUtf8("label_mm_8"));

        gridLayout_3->addWidget(label_mm_8, 3, 2, 1, 1);

        label_V_3 = new QLabel(centralwidget);
        label_V_3->setObjectName(QString::fromUtf8("label_V_3"));

        gridLayout_3->addWidget(label_V_3, 2, 3, 1, 1);

        label_ToolCo = new QLabel(centralwidget);
        label_ToolCo->setObjectName(QString::fromUtf8("label_ToolCo"));

        gridLayout_3->addWidget(label_ToolCo, 0, 0, 1, 6, Qt::AlignHCenter);

        LE_yToolCo = new QLineEdit(centralwidget);
        LE_yToolCo->setObjectName(QString::fromUtf8("LE_yToolCo"));

        gridLayout_3->addWidget(LE_yToolCo, 2, 1, 1, 1);

        LE_vToolCo = new QLineEdit(centralwidget);
        LE_vToolCo->setObjectName(QString::fromUtf8("LE_vToolCo"));

        gridLayout_3->addWidget(LE_vToolCo, 2, 4, 1, 1);

        label_W_3 = new QLabel(centralwidget);
        label_W_3->setObjectName(QString::fromUtf8("label_W_3"));

        gridLayout_3->addWidget(label_W_3, 3, 3, 1, 1);

        LE_uToolCo = new QLineEdit(centralwidget);
        LE_uToolCo->setObjectName(QString::fromUtf8("LE_uToolCo"));

        gridLayout_3->addWidget(LE_uToolCo, 1, 4, 1, 1);

        LE_wToolCo = new QLineEdit(centralwidget);
        LE_wToolCo->setObjectName(QString::fromUtf8("LE_wToolCo"));

        gridLayout_3->addWidget(LE_wToolCo, 3, 4, 1, 1);

        label_deg_9 = new QLabel(centralwidget);
        label_deg_9->setObjectName(QString::fromUtf8("label_deg_9"));

        gridLayout_3->addWidget(label_deg_9, 1, 5, 1, 1);

        label_Z_3 = new QLabel(centralwidget);
        label_Z_3->setObjectName(QString::fromUtf8("label_Z_3"));

        gridLayout_3->addWidget(label_Z_3, 3, 0, 1, 1);

        LE_zToolCo = new QLineEdit(centralwidget);
        LE_zToolCo->setObjectName(QString::fromUtf8("LE_zToolCo"));

        gridLayout_3->addWidget(LE_zToolCo, 3, 1, 1, 1);

        label_mm_7 = new QLabel(centralwidget);
        label_mm_7->setObjectName(QString::fromUtf8("label_mm_7"));

        gridLayout_3->addWidget(label_mm_7, 1, 2, 1, 1);

        label_X_2 = new QLabel(centralwidget);
        label_X_2->setObjectName(QString::fromUtf8("label_X_2"));

        gridLayout_3->addWidget(label_X_2, 1, 0, 1, 1);

        label_U_3 = new QLabel(centralwidget);
        label_U_3->setObjectName(QString::fromUtf8("label_U_3"));

        gridLayout_3->addWidget(label_U_3, 1, 3, 1, 1);

        LE_xToolCo = new QLineEdit(centralwidget);
        LE_xToolCo->setObjectName(QString::fromUtf8("LE_xToolCo"));

        gridLayout_3->addWidget(LE_xToolCo, 1, 1, 1, 1);

        label_deg_8 = new QLabel(centralwidget);
        label_deg_8->setObjectName(QString::fromUtf8("label_deg_8"));

        gridLayout_3->addWidget(label_deg_8, 2, 5, 1, 1);

        label_Y_3 = new QLabel(centralwidget);
        label_Y_3->setObjectName(QString::fromUtf8("label_Y_3"));

        gridLayout_3->addWidget(label_Y_3, 2, 0, 1, 1);

        label_deg_7 = new QLabel(centralwidget);
        label_deg_7->setObjectName(QString::fromUtf8("label_deg_7"));

        gridLayout_3->addWidget(label_deg_7, 3, 5, 1, 1);

        PB_setZeroCoSys = new QPushButton(centralwidget);
        PB_setZeroCoSys->setObjectName(QString::fromUtf8("PB_setZeroCoSys"));

        gridLayout_3->addWidget(PB_setZeroCoSys, 4, 0, 1, 6);


        gridLayout_5->addLayout(gridLayout_3, 1, 2, 1, 1);

        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        TE_LivePosition = new QTextEdit(centralwidget);
        TE_LivePosition->setObjectName(QString::fromUtf8("TE_LivePosition"));

        gridLayout_9->addWidget(TE_LivePosition, 0, 0, 1, 1);


        gridLayout_5->addLayout(gridLayout_9, 4, 2, 1, 3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        LE_vActiveCo = new QLineEdit(centralwidget);
        LE_vActiveCo->setObjectName(QString::fromUtf8("LE_vActiveCo"));

        gridLayout->addWidget(LE_vActiveCo, 2, 5, 1, 1);

        label_20 = new QLabel(centralwidget);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout->addWidget(label_20, 1, 0, 1, 1);

        LE_uActiveCo = new QLineEdit(centralwidget);
        LE_uActiveCo->setObjectName(QString::fromUtf8("LE_uActiveCo"));

        gridLayout->addWidget(LE_uActiveCo, 1, 5, 1, 1);

        LE_yActiveCo = new QLineEdit(centralwidget);
        LE_yActiveCo->setObjectName(QString::fromUtf8("LE_yActiveCo"));

        gridLayout->addWidget(LE_yActiveCo, 2, 1, 1, 1);

        label_22 = new QLabel(centralwidget);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout->addWidget(label_22, 3, 4, 1, 1);

        LE_xActiveCo = new QLineEdit(centralwidget);
        LE_xActiveCo->setObjectName(QString::fromUtf8("LE_xActiveCo"));

        gridLayout->addWidget(LE_xActiveCo, 1, 1, 1, 1);

        label_18 = new QLabel(centralwidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 2, 4, 1, 1);

        label_38 = new QLabel(centralwidget);
        label_38->setObjectName(QString::fromUtf8("label_38"));

        gridLayout->addWidget(label_38, 1, 6, 1, 1);

        label_36 = new QLabel(centralwidget);
        label_36->setObjectName(QString::fromUtf8("label_36"));

        gridLayout->addWidget(label_36, 2, 3, 1, 1);

        label_21 = new QLabel(centralwidget);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout->addWidget(label_21, 2, 0, 1, 1);

        label_35 = new QLabel(centralwidget);
        label_35->setObjectName(QString::fromUtf8("label_35"));

        gridLayout->addWidget(label_35, 1, 3, 1, 1);

        label_19 = new QLabel(centralwidget);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout->addWidget(label_19, 3, 0, 1, 1);

        LE_zActiveCo = new QLineEdit(centralwidget);
        LE_zActiveCo->setObjectName(QString::fromUtf8("LE_zActiveCo"));

        gridLayout->addWidget(LE_zActiveCo, 3, 1, 1, 1);

        label_17 = new QLabel(centralwidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 1, 4, 1, 1);

        label_40 = new QLabel(centralwidget);
        label_40->setObjectName(QString::fromUtf8("label_40"));

        gridLayout->addWidget(label_40, 3, 6, 1, 1);

        label_37 = new QLabel(centralwidget);
        label_37->setObjectName(QString::fromUtf8("label_37"));

        gridLayout->addWidget(label_37, 3, 3, 1, 1);

        label_39 = new QLabel(centralwidget);
        label_39->setObjectName(QString::fromUtf8("label_39"));

        gridLayout->addWidget(label_39, 2, 6, 1, 1);

        LE_wActiveCo = new QLineEdit(centralwidget);
        LE_wActiveCo->setObjectName(QString::fromUtf8("LE_wActiveCo"));

        gridLayout->addWidget(LE_wActiveCo, 3, 5, 1, 1);

        PB_ZeroOnActiveCoSys = new QPushButton(centralwidget);
        PB_ZeroOnActiveCoSys->setObjectName(QString::fromUtf8("PB_ZeroOnActiveCoSys"));

        gridLayout->addWidget(PB_ZeroOnActiveCoSys, 4, 0, 1, 7);

        label_23 = new QLabel(centralwidget);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout->addWidget(label_23, 0, 0, 1, 7, Qt::AlignHCenter);


        gridLayout_5->addLayout(gridLayout, 1, 0, 1, 1);

        gridLayout_StabilizationSettings = new QGridLayout();
        gridLayout_StabilizationSettings->setObjectName(QString::fromUtf8("gridLayout_StabilizationSettings"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        integrationTimeLabel = new QLabel(centralwidget);
        integrationTimeLabel->setObjectName(QString::fromUtf8("integrationTimeLabel"));
        integrationTimeLabel->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(integrationTimeLabel, 2, 0, 1, 1);

        deadTimeLabel = new QLabel(centralwidget);
        deadTimeLabel->setObjectName(QString::fromUtf8("deadTimeLabel"));
        deadTimeLabel->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(deadTimeLabel, 3, 0, 1, 1);

        proportionalGainLabel = new QLabel(centralwidget);
        proportionalGainLabel->setObjectName(QString::fromUtf8("proportionalGainLabel"));
        proportionalGainLabel->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(proportionalGainLabel, 4, 0, 1, 1);

        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMinimumSize(QSize(0, 0));

        gridLayout_8->addWidget(label_10, 2, 2, 1, 1);

        SB_integrationTime = new QSpinBox(centralwidget);
        SB_integrationTime->setObjectName(QString::fromUtf8("SB_integrationTime"));
        SB_integrationTime->setMaximum(10);

        gridLayout_8->addWidget(SB_integrationTime, 2, 1, 1, 1);

        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMinimumSize(QSize(0, 0));

        gridLayout_8->addWidget(label_11, 5, 2, 1, 1);

        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setMinimumSize(QSize(0, 0));

        gridLayout_8->addWidget(label_9, 3, 2, 1, 1);

        maxMotionLabel = new QLabel(centralwidget);
        maxMotionLabel->setObjectName(QString::fromUtf8("maxMotionLabel"));
        maxMotionLabel->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(maxMotionLabel, 5, 0, 1, 1);

        TE_LogHexapod = new QTextEdit(centralwidget);
        TE_LogHexapod->setObjectName(QString::fromUtf8("TE_LogHexapod"));

        gridLayout_8->addWidget(TE_LogHexapod, 0, 0, 1, 3);

        SB_deadTime = new QSpinBox(centralwidget);
        SB_deadTime->setObjectName(QString::fromUtf8("SB_deadTime"));
        SB_deadTime->setMaximum(10);

        gridLayout_8->addWidget(SB_deadTime, 3, 1, 1, 1);

        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_8->addWidget(label_8, 1, 0, 1, 3, Qt::AlignHCenter);

        SB_proportionalGain = new QDoubleSpinBox(centralwidget);
        SB_proportionalGain->setObjectName(QString::fromUtf8("SB_proportionalGain"));
        SB_proportionalGain->setMaximum(1.000000000000000);
        SB_proportionalGain->setSingleStep(0.100000000000000);

        gridLayout_8->addWidget(SB_proportionalGain, 4, 1, 1, 1);

        SB_maxMotion = new QDoubleSpinBox(centralwidget);
        SB_maxMotion->setObjectName(QString::fromUtf8("SB_maxMotion"));
        SB_maxMotion->setDecimals(4);

        gridLayout_8->addWidget(SB_maxMotion, 5, 1, 1, 1);

        gridLayout_8->setColumnStretch(0, 4);
        gridLayout_8->setColumnStretch(1, 2);
        gridLayout_8->setColumnStretch(2, 1);

        gridLayout_StabilizationSettings->addLayout(gridLayout_8, 0, 2, 1, 1);


        gridLayout_5->addLayout(gridLayout_StabilizationSettings, 4, 1, 1, 1);

        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        LE_uAbsCo = new QLineEdit(centralwidget);
        LE_uAbsCo->setObjectName(QString::fromUtf8("LE_uAbsCo"));

        gridLayout_6->addWidget(LE_uAbsCo, 1, 4, 1, 1);

        label_Y_2 = new QLabel(centralwidget);
        label_Y_2->setObjectName(QString::fromUtf8("label_Y_2"));

        gridLayout_6->addWidget(label_Y_2, 2, 0, 1, 1);

        label_AbsCoord = new QLabel(centralwidget);
        label_AbsCoord->setObjectName(QString::fromUtf8("label_AbsCoord"));

        gridLayout_6->addWidget(label_AbsCoord, 0, 0, 1, 6, Qt::AlignHCenter|Qt::AlignVCenter);

        label_X_3 = new QLabel(centralwidget);
        label_X_3->setObjectName(QString::fromUtf8("label_X_3"));

        gridLayout_6->addWidget(label_X_3, 1, 0, 1, 1);

        LE_wAbsCo = new QLineEdit(centralwidget);
        LE_wAbsCo->setObjectName(QString::fromUtf8("LE_wAbsCo"));

        gridLayout_6->addWidget(LE_wAbsCo, 3, 4, 1, 1);

        label_mm_5 = new QLabel(centralwidget);
        label_mm_5->setObjectName(QString::fromUtf8("label_mm_5"));

        gridLayout_6->addWidget(label_mm_5, 3, 2, 1, 1);

        label_mm_4 = new QLabel(centralwidget);
        label_mm_4->setObjectName(QString::fromUtf8("label_mm_4"));

        gridLayout_6->addWidget(label_mm_4, 1, 2, 1, 1);

        label_mm_6 = new QLabel(centralwidget);
        label_mm_6->setObjectName(QString::fromUtf8("label_mm_6"));

        gridLayout_6->addWidget(label_mm_6, 2, 2, 1, 1);

        LE_zAbsCo = new QLineEdit(centralwidget);
        LE_zAbsCo->setObjectName(QString::fromUtf8("LE_zAbsCo"));

        gridLayout_6->addWidget(LE_zAbsCo, 3, 1, 1, 1);

        label_W_2 = new QLabel(centralwidget);
        label_W_2->setObjectName(QString::fromUtf8("label_W_2"));

        gridLayout_6->addWidget(label_W_2, 3, 3, 1, 1);

        label_U_2 = new QLabel(centralwidget);
        label_U_2->setObjectName(QString::fromUtf8("label_U_2"));

        gridLayout_6->addWidget(label_U_2, 1, 3, 1, 1);

        LE_yAbsCo = new QLineEdit(centralwidget);
        LE_yAbsCo->setObjectName(QString::fromUtf8("LE_yAbsCo"));

        gridLayout_6->addWidget(LE_yAbsCo, 2, 1, 1, 1);

        LE_vAbsCo = new QLineEdit(centralwidget);
        LE_vAbsCo->setObjectName(QString::fromUtf8("LE_vAbsCo"));

        gridLayout_6->addWidget(LE_vAbsCo, 2, 4, 1, 1);

        label_deg_4 = new QLabel(centralwidget);
        label_deg_4->setObjectName(QString::fromUtf8("label_deg_4"));

        gridLayout_6->addWidget(label_deg_4, 2, 5, 1, 1);

        LE_xAbsCo = new QLineEdit(centralwidget);
        LE_xAbsCo->setObjectName(QString::fromUtf8("LE_xAbsCo"));

        gridLayout_6->addWidget(LE_xAbsCo, 1, 1, 1, 1);

        label_deg_6 = new QLabel(centralwidget);
        label_deg_6->setObjectName(QString::fromUtf8("label_deg_6"));

        gridLayout_6->addWidget(label_deg_6, 3, 5, 1, 1);

        label_V_2 = new QLabel(centralwidget);
        label_V_2->setObjectName(QString::fromUtf8("label_V_2"));

        gridLayout_6->addWidget(label_V_2, 2, 3, 1, 1);

        label_Z_2 = new QLabel(centralwidget);
        label_Z_2->setObjectName(QString::fromUtf8("label_Z_2"));

        gridLayout_6->addWidget(label_Z_2, 3, 0, 1, 1);

        label_deg_5 = new QLabel(centralwidget);
        label_deg_5->setObjectName(QString::fromUtf8("label_deg_5"));

        gridLayout_6->addWidget(label_deg_5, 1, 5, 1, 1);

        PB_setUserDefinedCoSys = new QPushButton(centralwidget);
        PB_setUserDefinedCoSys->setObjectName(QString::fromUtf8("PB_setUserDefinedCoSys"));

        gridLayout_6->addWidget(PB_setUserDefinedCoSys, 4, 0, 1, 6);


        gridLayout_5->addLayout(gridLayout_6, 1, 1, 1, 1);

        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_6, 5, 0, 1, 1);

        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_7, 5, 1, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);
        label_2->setIndent(-1);

        gridLayout_7->addWidget(label_2, 3, 0, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_7->addWidget(label, 0, 0, 1, 2, Qt::AlignHCenter);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_4, 4, 0, 1, 1);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_5, 4, 1, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_7->addWidget(label_3, 3, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_StepSize = new QLabel(centralwidget);
        label_StepSize->setObjectName(QString::fromUtf8("label_StepSize"));

        horizontalLayout_4->addWidget(label_StepSize);

        HS_IncrementSize = new QSlider(centralwidget);
        HS_IncrementSize->setObjectName(QString::fromUtf8("HS_IncrementSize"));
        HS_IncrementSize->setMinimum(1);
        HS_IncrementSize->setMaximum(10000);
        HS_IncrementSize->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(HS_IncrementSize);

        SB_StepSize = new QSpinBox(centralwidget);
        SB_StepSize->setObjectName(QString::fromUtf8("SB_StepSize"));
        SB_StepSize->setMinimum(1);
        SB_StepSize->setMaximum(1000);

        horizontalLayout_4->addWidget(SB_StepSize);

        label_12 = new QLabel(centralwidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_4->addWidget(label_12);


        gridLayout_7->addLayout(horizontalLayout_4, 6, 0, 1, 2);


        gridLayout_5->addLayout(gridLayout_7, 4, 0, 1, 1);

        HexapodGui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(HexapodGui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 2119, 39));
        menuHexapod_GUI = new QMenu(menubar);
        menuHexapod_GUI->setObjectName(QString::fromUtf8("menuHexapod_GUI"));
        HexapodGui->setMenuBar(menubar);
        statusbar = new QStatusBar(HexapodGui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        HexapodGui->setStatusBar(statusbar);

        menubar->addAction(menuHexapod_GUI->menuAction());

        retranslateUi(HexapodGui);

        QMetaObject::connectSlotsByName(HexapodGui);
    } // setupUi

    void retranslateUi(QMainWindow *HexapodGui)
    {
        HexapodGui->setWindowTitle(QApplication::translate("HexapodGui", "MainWindow", nullptr));
        label_U->setText(QApplication::translate("HexapodGui", "U", nullptr));
        label_deg_3->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_V->setText(QApplication::translate("HexapodGui", "V", nullptr));
        label_mm_3->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_W->setText(QApplication::translate("HexapodGui", "W", nullptr));
        label_mm_2->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_Z->setText(QApplication::translate("HexapodGui", "Z", nullptr));
        label_mm->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_deg->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_MoveToPos->setText(QApplication::translate("HexapodGui", "Move to pos (rel. active coord. syst.)", nullptr));
        label_deg_2->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_Y->setText(QApplication::translate("HexapodGui", "Y", nullptr));
        label_X->setText(QApplication::translate("HexapodGui", "X", nullptr));
        PB_MoveToPosition->setText(QApplication::translate("HexapodGui", "Move to position", nullptr));
        label_mm_9->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_mm_8->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_V_3->setText(QApplication::translate("HexapodGui", "V", nullptr));
        label_ToolCo->setText(QApplication::translate("HexapodGui", "Tool Coordinates (rel. active coord. syst)", nullptr));
        label_W_3->setText(QApplication::translate("HexapodGui", "W", nullptr));
        label_deg_9->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_Z_3->setText(QApplication::translate("HexapodGui", "Z", nullptr));
        label_mm_7->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_X_2->setText(QApplication::translate("HexapodGui", "X", nullptr));
        label_U_3->setText(QApplication::translate("HexapodGui", "U", nullptr));
        label_deg_8->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_Y_3->setText(QApplication::translate("HexapodGui", "Y", nullptr));
        label_deg_7->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        PB_setZeroCoSys->setText(QApplication::translate("HexapodGui", "Activate ZERO coordinate system", nullptr));
        label_20->setText(QApplication::translate("HexapodGui", "X", nullptr));
        label_22->setText(QApplication::translate("HexapodGui", "W", nullptr));
        label_18->setText(QApplication::translate("HexapodGui", "V", nullptr));
        label_38->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_36->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_21->setText(QApplication::translate("HexapodGui", "Y", nullptr));
        label_35->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_19->setText(QApplication::translate("HexapodGui", "Z", nullptr));
        label_17->setText(QApplication::translate("HexapodGui", "U", nullptr));
        label_40->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_37->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_39->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        PB_ZeroOnActiveCoSys->setText(QApplication::translate("HexapodGui", "Zero on active coordinate system", nullptr));
        label_23->setText(QApplication::translate("HexapodGui", "Active Coordinate System (rel. ZERO)", nullptr));
        integrationTimeLabel->setText(QApplication::translate("HexapodGui", "IntegrationTime", nullptr));
        deadTimeLabel->setText(QApplication::translate("HexapodGui", "Dead Time ", nullptr));
        proportionalGainLabel->setText(QApplication::translate("HexapodGui", "Proportional Gain", nullptr));
        label_10->setText(QApplication::translate("HexapodGui", "s", nullptr));
        label_11->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_9->setText(QApplication::translate("HexapodGui", "s", nullptr));
        maxMotionLabel->setText(QApplication::translate("HexapodGui", "Max motion", nullptr));
        TE_LogHexapod->setHtml(QApplication::translate("HexapodGui", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Click here when moving manually with keyboard</p></body></html>", nullptr));
        label_8->setText(QApplication::translate("HexapodGui", "Stabilization parameters", nullptr));
        label_Y_2->setText(QApplication::translate("HexapodGui", "Y", nullptr));
        label_AbsCoord->setText(QApplication::translate("HexapodGui", "Absolute Coordinates (rel. ZERO)", nullptr));
        label_X_3->setText(QApplication::translate("HexapodGui", "X", nullptr));
        label_mm_5->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_mm_4->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_mm_6->setText(QApplication::translate("HexapodGui", "mm", nullptr));
        label_W_2->setText(QApplication::translate("HexapodGui", "W", nullptr));
        label_U_2->setText(QApplication::translate("HexapodGui", "U", nullptr));
        label_deg_4->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_deg_6->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        label_V_2->setText(QApplication::translate("HexapodGui", "V", nullptr));
        label_Z_2->setText(QApplication::translate("HexapodGui", "Z", nullptr));
        label_deg_5->setText(QApplication::translate("HexapodGui", "deg", nullptr));
        PB_setUserDefinedCoSys->setText(QApplication::translate("HexapodGui", "Set KSF coordinate system at current pos ", nullptr));
        label_6->setText(QApplication::translate("HexapodGui", "Z: Q-E", nullptr));
        label_7->setText(QApplication::translate("HexapodGui", "W: K-L", nullptr));
        label_2->setText(QApplication::translate("HexapodGui", "X: A-D", nullptr));
        label->setText(QApplication::translate("HexapodGui", "Manual control keys (+-)", nullptr));
        label_4->setText(QApplication::translate("HexapodGui", "Y: W-S", nullptr));
        label_5->setText(QApplication::translate("HexapodGui", "V: V-B", nullptr));
        label_3->setText(QApplication::translate("HexapodGui", "U: U-I", nullptr));
        label_StepSize->setText(QApplication::translate("HexapodGui", "Step size", nullptr));
        label_12->setText(QApplication::translate("HexapodGui", "x min. inc.", nullptr));
        menuHexapod_GUI->setTitle(QApplication::translate("HexapodGui", "Hexapod GUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HexapodGui: public Ui_HexapodGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEXAPODGUI_H
