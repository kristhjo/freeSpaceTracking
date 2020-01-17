/********************************************************************************
** Form generated from reading UI file 'seeinggui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEEINGGUI_H
#define UI_SEEINGGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SeeingGui
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QGridLayout *gridLayout_config;
    QHBoxLayout *horizontalLayout_7;
    QSpinBox *SB_wavelength;
    QLabel *Label_nanometer;
    QComboBox *ComboB_MeasurementType;
    QHBoxLayout *horizontalLayout_11;
    QLabel *Label_WindowingRadius;
    QCheckBox *checkB_useWindow;
    QHBoxLayout *horizontalLayout_2;
    QLabel *Label_Debug;
    QCheckBox *CheckB_Debug;
    QHBoxLayout *horizontalLayout_4;
    QSpinBox *SB_Threshold;
    QLabel *label_grayscale;
    QHBoxLayout *horizontalLayout_6;
    QSpinBox *SB_SampleSize;
    QLineEdit *LE_SecPerDataPoint;
    QLabel *Label_SecPerDataPoint;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *SB_WindowingRadius;
    QLabel *label;
    QSpinBox *SB_airyMin;
    QLabel *label_airymin;
    QLabel *label_3;
    QLabel *Label_SeeingMeasurementType;
    QLabel *label_wavelength;
    QHBoxLayout *horizontalLayout;
    QLabel *Label_StoreImages;
    QCheckBox *CheckB_StoreImages;
    QLabel *label_2;
    QLabel *Label_StorageLocation;
    QLabel *Label_SampleSize;
    QLineEdit *LE_DirectoryPath;
    QSpinBox *SB_saturationCutOff;
    QHBoxLayout *horizontalLayout_12;
    QLabel *Label_WindowingRadius_2;
    QCheckBox *checkB_useThreshold;
    QHBoxLayout *horizontalLayout_5;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_4;
    QTextEdit *TE_logSeeing;
    QMenuBar *menubar;
    QMenu *menuSeeingGui;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SeeingGui)
    {
        if (SeeingGui->objectName().isEmpty())
            SeeingGui->setObjectName(QString::fromUtf8("SeeingGui"));
        SeeingGui->resize(859, 792);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SeeingGui->sizePolicy().hasHeightForWidth());
        SeeingGui->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(SeeingGui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout_config = new QGridLayout();
        gridLayout_config->setObjectName(QString::fromUtf8("gridLayout_config"));
        gridLayout_config->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        SB_wavelength = new QSpinBox(centralwidget);
        SB_wavelength->setObjectName(QString::fromUtf8("SB_wavelength"));

        horizontalLayout_7->addWidget(SB_wavelength);

        Label_nanometer = new QLabel(centralwidget);
        Label_nanometer->setObjectName(QString::fromUtf8("Label_nanometer"));

        horizontalLayout_7->addWidget(Label_nanometer);


        gridLayout_config->addLayout(horizontalLayout_7, 6, 2, 1, 1);

        ComboB_MeasurementType = new QComboBox(centralwidget);
        ComboB_MeasurementType->addItem(QString());
        ComboB_MeasurementType->addItem(QString());
        ComboB_MeasurementType->setObjectName(QString::fromUtf8("ComboB_MeasurementType"));

        gridLayout_config->addWidget(ComboB_MeasurementType, 0, 2, 1, 1);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        Label_WindowingRadius = new QLabel(centralwidget);
        Label_WindowingRadius->setObjectName(QString::fromUtf8("Label_WindowingRadius"));

        horizontalLayout_11->addWidget(Label_WindowingRadius);

        checkB_useWindow = new QCheckBox(centralwidget);
        checkB_useWindow->setObjectName(QString::fromUtf8("checkB_useWindow"));

        horizontalLayout_11->addWidget(checkB_useWindow);


        gridLayout_config->addLayout(horizontalLayout_11, 4, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Label_Debug = new QLabel(centralwidget);
        Label_Debug->setObjectName(QString::fromUtf8("Label_Debug"));

        horizontalLayout_2->addWidget(Label_Debug);

        CheckB_Debug = new QCheckBox(centralwidget);
        CheckB_Debug->setObjectName(QString::fromUtf8("CheckB_Debug"));

        horizontalLayout_2->addWidget(CheckB_Debug);


        gridLayout_config->addLayout(horizontalLayout_2, 2, 2, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        SB_Threshold = new QSpinBox(centralwidget);
        SB_Threshold->setObjectName(QString::fromUtf8("SB_Threshold"));

        horizontalLayout_4->addWidget(SB_Threshold);

        label_grayscale = new QLabel(centralwidget);
        label_grayscale->setObjectName(QString::fromUtf8("label_grayscale"));

        horizontalLayout_4->addWidget(label_grayscale);


        gridLayout_config->addLayout(horizontalLayout_4, 5, 2, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        SB_SampleSize = new QSpinBox(centralwidget);
        SB_SampleSize->setObjectName(QString::fromUtf8("SB_SampleSize"));

        horizontalLayout_6->addWidget(SB_SampleSize);

        LE_SecPerDataPoint = new QLineEdit(centralwidget);
        LE_SecPerDataPoint->setObjectName(QString::fromUtf8("LE_SecPerDataPoint"));

        horizontalLayout_6->addWidget(LE_SecPerDataPoint);

        Label_SecPerDataPoint = new QLabel(centralwidget);
        Label_SecPerDataPoint->setObjectName(QString::fromUtf8("Label_SecPerDataPoint"));

        horizontalLayout_6->addWidget(Label_SecPerDataPoint);


        gridLayout_config->addLayout(horizontalLayout_6, 1, 2, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        SB_WindowingRadius = new QSpinBox(centralwidget);
        SB_WindowingRadius->setObjectName(QString::fromUtf8("SB_WindowingRadius"));

        horizontalLayout_3->addWidget(SB_WindowingRadius);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_3->addWidget(label);

        SB_airyMin = new QSpinBox(centralwidget);
        SB_airyMin->setObjectName(QString::fromUtf8("SB_airyMin"));

        horizontalLayout_3->addWidget(SB_airyMin);

        label_airymin = new QLabel(centralwidget);
        label_airymin->setObjectName(QString::fromUtf8("label_airymin"));

        horizontalLayout_3->addWidget(label_airymin);


        gridLayout_config->addLayout(horizontalLayout_3, 4, 2, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_config->addWidget(label_3, 8, 0, 1, 1);

        Label_SeeingMeasurementType = new QLabel(centralwidget);
        Label_SeeingMeasurementType->setObjectName(QString::fromUtf8("Label_SeeingMeasurementType"));

        gridLayout_config->addWidget(Label_SeeingMeasurementType, 0, 0, 1, 1);

        label_wavelength = new QLabel(centralwidget);
        label_wavelength->setObjectName(QString::fromUtf8("label_wavelength"));

        gridLayout_config->addWidget(label_wavelength, 6, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Label_StoreImages = new QLabel(centralwidget);
        Label_StoreImages->setObjectName(QString::fromUtf8("Label_StoreImages"));

        horizontalLayout->addWidget(Label_StoreImages);

        CheckB_StoreImages = new QCheckBox(centralwidget);
        CheckB_StoreImages->setObjectName(QString::fromUtf8("CheckB_StoreImages"));

        horizontalLayout->addWidget(CheckB_StoreImages);


        gridLayout_config->addLayout(horizontalLayout, 2, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_config->addWidget(label_2, 7, 0, 1, 1);

        Label_StorageLocation = new QLabel(centralwidget);
        Label_StorageLocation->setObjectName(QString::fromUtf8("Label_StorageLocation"));

        gridLayout_config->addWidget(Label_StorageLocation, 3, 0, 1, 1);

        Label_SampleSize = new QLabel(centralwidget);
        Label_SampleSize->setObjectName(QString::fromUtf8("Label_SampleSize"));

        gridLayout_config->addWidget(Label_SampleSize, 1, 0, 1, 1);

        LE_DirectoryPath = new QLineEdit(centralwidget);
        LE_DirectoryPath->setObjectName(QString::fromUtf8("LE_DirectoryPath"));

        gridLayout_config->addWidget(LE_DirectoryPath, 3, 2, 1, 1);

        SB_saturationCutOff = new QSpinBox(centralwidget);
        SB_saturationCutOff->setObjectName(QString::fromUtf8("SB_saturationCutOff"));

        gridLayout_config->addWidget(SB_saturationCutOff, 7, 2, 1, 1);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        Label_WindowingRadius_2 = new QLabel(centralwidget);
        Label_WindowingRadius_2->setObjectName(QString::fromUtf8("Label_WindowingRadius_2"));

        horizontalLayout_12->addWidget(Label_WindowingRadius_2);

        checkB_useThreshold = new QCheckBox(centralwidget);
        checkB_useThreshold->setObjectName(QString::fromUtf8("checkB_useThreshold"));

        horizontalLayout_12->addWidget(checkB_useThreshold);


        gridLayout_config->addLayout(horizontalLayout_12, 5, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        doubleSpinBox = new QDoubleSpinBox(centralwidget);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));

        horizontalLayout_5->addWidget(doubleSpinBox);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_5->addWidget(label_4);


        gridLayout_config->addLayout(horizontalLayout_5, 8, 2, 1, 1);


        gridLayout->addLayout(gridLayout_config, 0, 0, 1, 1);

        TE_logSeeing = new QTextEdit(centralwidget);
        TE_logSeeing->setObjectName(QString::fromUtf8("TE_logSeeing"));
        sizePolicy.setHeightForWidth(TE_logSeeing->sizePolicy().hasHeightForWidth());
        TE_logSeeing->setSizePolicy(sizePolicy);

        gridLayout->addWidget(TE_logSeeing, 1, 0, 1, 1);

        SeeingGui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SeeingGui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 859, 39));
        menuSeeingGui = new QMenu(menubar);
        menuSeeingGui->setObjectName(QString::fromUtf8("menuSeeingGui"));
        SeeingGui->setMenuBar(menubar);
        statusbar = new QStatusBar(SeeingGui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        SeeingGui->setStatusBar(statusbar);

        menubar->addAction(menuSeeingGui->menuAction());

        retranslateUi(SeeingGui);

        QMetaObject::connectSlotsByName(SeeingGui);
    } // setupUi

    void retranslateUi(QMainWindow *SeeingGui)
    {
        SeeingGui->setWindowTitle(QApplication::translate("SeeingGui", "MainWindow", nullptr));
        Label_nanometer->setText(QApplication::translate("SeeingGui", "nm", nullptr));
        ComboB_MeasurementType->setItemText(0, QApplication::translate("SeeingGui", "Gaussian fit", nullptr));
        ComboB_MeasurementType->setItemText(1, QApplication::translate("SeeingGui", "DIMM", nullptr));

        Label_WindowingRadius->setText(QApplication::translate("SeeingGui", "Windowing Radius", nullptr));
        checkB_useWindow->setText(QApplication::translate("SeeingGui", "use", nullptr));
        Label_Debug->setText(QApplication::translate("SeeingGui", "Debug", nullptr));
        CheckB_Debug->setText(QApplication::translate("SeeingGui", "Yes", nullptr));
        label_grayscale->setText(QApplication::translate("SeeingGui", "0-255 greyscale", nullptr));
        Label_SecPerDataPoint->setText(QApplication::translate("SeeingGui", "s per data point", nullptr));
        label->setText(QApplication::translate("SeeingGui", "px", nullptr));
        label_airymin->setText(QApplication::translate("SeeingGui", "# airy min.", nullptr));
        label_3->setText(QApplication::translate("SeeingGui", "Strehl ratio cut-off", nullptr));
        Label_SeeingMeasurementType->setText(QApplication::translate("SeeingGui", "Seeing measurement type", nullptr));
        label_wavelength->setText(QApplication::translate("SeeingGui", "Wavelength", nullptr));
        Label_StoreImages->setText(QApplication::translate("SeeingGui", "Store images", nullptr));
        CheckB_StoreImages->setText(QApplication::translate("SeeingGui", "Yes", nullptr));
        label_2->setText(QApplication::translate("SeeingGui", "# Saturated pixels cut-off", nullptr));
        Label_StorageLocation->setText(QApplication::translate("SeeingGui", "Storage location", nullptr));
        Label_SampleSize->setText(QApplication::translate("SeeingGui", "Sample size per data point", nullptr));
        LE_DirectoryPath->setText(QApplication::translate("SeeingGui", "/home/domi/Desktop/seeingTests", nullptr));
        Label_WindowingRadius_2->setText(QApplication::translate("SeeingGui", "Thresholding intensity", nullptr));
        checkB_useThreshold->setText(QApplication::translate("SeeingGui", "use", nullptr));
        label_4->setText(QApplication::translate("SeeingGui", "(for DIMM only)", nullptr));
        menuSeeingGui->setTitle(QApplication::translate("SeeingGui", "SeeingGui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SeeingGui: public Ui_SeeingGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEEINGGUI_H
