/********************************************************************************
** Form generated from reading UI file 'livedisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIVEDISPLAY_H
#define UI_LIVEDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_liveDisplay
{
public:

    void setupUi(QWidget *liveDisplay)
    {
        if (liveDisplay->objectName().isEmpty())
            liveDisplay->setObjectName(QString::fromUtf8("liveDisplay"));
        liveDisplay->resize(400, 300);

        retranslateUi(liveDisplay);

        QMetaObject::connectSlotsByName(liveDisplay);
    } // setupUi

    void retranslateUi(QWidget *liveDisplay)
    {
        liveDisplay->setWindowTitle(QApplication::translate("liveDisplay", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class liveDisplay: public Ui_liveDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIVEDISPLAY_H
