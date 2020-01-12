#ifndef LIVEDISPLAY_H
#define LIVEDISPLAY_H

#include <QWidget>
#include <iostream>
#include <qcustomplot.h>


namespace Ui {
class liveDisplay;
}

struct layoutPosition1{
    layoutPosition1():rowPos(0), colPos(0), rowSpan(1), colSpan(1){}
    layoutPosition1(int row, int col, int rSpan, int cSpan):rowPos(row), colPos(col), rowSpan(rSpan),  colSpan(cSpan){}
    int rowPos;
    int colPos;
    int rowSpan;
    int colSpan;
};

struct displayConfiguration{
    displayConfiguration(QString label, int precision, char format, layoutPosition1 lpos): text_label(label), displayPrecision(precision), displayFormat(format), pos(lpos){}
    QString text_label;
    double LE_value;
    int displayPrecision;
    char displayFormat;
    layoutPosition1 pos;
    QString unit_label;
    QMap<QString, std::shared_ptr<double>> LE_col;
};




class liveDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit liveDisplay(QWidget *parent = nullptr);
    explicit liveDisplay(QMap<QString, std::shared_ptr<displayConfiguration>> displays, QWidget *parent = nullptr);

    ~liveDisplay();
    void updateDisplay();
    void add_display(displayConfiguration config);
private:

    QGridLayout *layout;
    QMap<QString, std::shared_ptr<displayConfiguration>> display_register;
    Ui::liveDisplay *ui;
};

#endif // LIVEDISPLAY_H
