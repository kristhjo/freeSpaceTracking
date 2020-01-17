/********************************************************************************
** Form generated from reading UI file 'plotdisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTDISPLAY_H
#define UI_PLOTDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_plotDisplay
{
public:

    void setupUi(QWidget *plotDisplay)
    {
        if (plotDisplay->objectName().isEmpty())
            plotDisplay->setObjectName(QString::fromUtf8("plotDisplay"));
        plotDisplay->resize(400, 300);

        retranslateUi(plotDisplay);

        QMetaObject::connectSlotsByName(plotDisplay);
    } // setupUi

    void retranslateUi(QWidget *plotDisplay)
    {
        plotDisplay->setWindowTitle(QApplication::translate("plotDisplay", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class plotDisplay: public Ui_plotDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTDISPLAY_H
