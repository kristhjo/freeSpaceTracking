#ifndef PLOTDISPLAY_H
#define PLOTDISPLAY_H

#include <QWidget>
#include <iostream>
#include <QVector>
#include <qcustomplot.h>
#include <opencv2/opencv.hpp>

namespace Ui {
class plotDisplay;
}

struct layoutPosition{
    layoutPosition():rowPos(0), colPos(0), rowSpan(1), colSpan(1){}
    layoutPosition(int row, int col, int rSpan, int cSpan):rowPos(row), colPos(col), rowSpan(rSpan),  colSpan(cSpan){}
    int rowPos;
    int colPos;
    int rowSpan;
    int colSpan;
};

struct plotFonts{
    plotFonts() : titleFont(QFont("sans", 12, QFont::Bold)), legendFont(QFont("sans-serif", 8, QFont::Bold)), axisFont(QFont("sans", 12, QFont::Bold)){}
    plotFonts(QFont title, QFont legend, QFont axis) : titleFont(title), legendFont(legend), axisFont(axis) {}
    QFont titleFont;
    QFont legendFont;
    QFont axisFont;
};

struct savingSettings{
    savingSettings() : save_plot(false) {}
    savingSettings(QString file_name, bool save_plot = true, QString type_name = "png", int x_dim = 400, int y_dim = 400)
        : save_plot(save_plot), file_name(file_name), type_name(type_name), x_dim(x_dim), y_dim(y_dim){}
    bool save_plot;
    QString file_name;
    QString type_name; //"png, pdf, jpg..."
    int x_dim;
    int y_dim;
};


struct labelContainer{
    labelContainer(QString x="", QString y="", QString title="", QString x2 = "", QString y2 = "")
        : xAxisLabel(x), yAxisLabel(y), title(title), xAxis2Label(x2), yAxis2Label(y2){}
    QString xAxisLabel;
    QString yAxisLabel;
    QString title;
    QString xAxis2Label;
    QString yAxis2Label;
};



struct scatterPlotConfig{
    scatterPlotConfig(QSharedPointer<QCPGraphDataContainer> data_ptr, QString legend = "", QCPGraph::LineStyle ls = QCPGraph::lsNone,
                      QCPScatterStyle scstyle = QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5), QPen pen = QPen(Qt::red, 2 , Qt::DotLine) )
        : data(data_ptr), penstyle(pen), legendText(legend), linestyle(ls), scatterStyle(scstyle){}
    QSharedPointer<QCPGraphDataContainer> data;
    QCPGraph* scatterPlot;
    double yAxisHighRangeFactor = 1.2;
    double yAxisLowRangeFactor = 0.8;
    double rangeMargin = 0.2;
    QPen penstyle;
    QString legendText;
    QCPGraph::LineStyle linestyle;
    QCPScatterStyle scatterStyle;
    QSharedPointer<QCPAxisTickerDateTime> dateTicker;
    QCPAxis::AxisType xaxes =QCPAxis::atBottom;
    QCPAxis::AxisType yaxes =QCPAxis::atLeft;
    //legend
};


struct histogramConfig{
    histogramConfig():data(nullptr), histogram(nullptr){}
    histogramConfig(QSharedPointer<QCPBarsDataContainer> data_ptr): data(data_ptr), histogram(nullptr), brushStyle(QColor(10, 140, 70, 160)){}
    QSharedPointer<QCPBarsDataContainer> data;
    QCPBars* histogram;
    QBrush brushStyle;

};


struct colormapConfig{
    colormapConfig(): data(nullptr), colormap(nullptr), colorscale(nullptr){}
    QCPColorMapData* data;
    QCPColorMap* colormap;
    QCPColorScale* colorscale;

};

class plot{
   public:
   explicit plot(layoutPosition lPos = layoutPosition(), QString name = "",
                       labelContainer labs = labelContainer("x","y","title","",""),
                       savingSettings save = savingSettings(), plotFonts fonts = plotFonts())
        : plotName(name), pos(lPos), fontStyles(fonts), save_settings(save), labels(labs){}

    QString plotName;
    layoutPosition pos;
    plotFonts fontStyles;
    savingSettings save_settings;
    labelContainer labels;
    QCustomPlot* customPlot;
    QCPItemText* item;
    inline void add_itembox(){
        if(this->customPlot){
            this->item = new QCPItemText(this->customPlot);
            item->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
            item->position->setType(QCPItemPosition::ptAxisRectRatio);
            item->position->setCoords(0.5, 0); // place position at center/top of axis rect
            item->setFont(this->fontStyles.legendFont);
            item->setPen(QPen(Qt::white));
            item->setBrush(QBrush(QColor(245,245,220, 100)));
            item->setPadding(QMargins(3,3,3,3));
        }
    }

};

class scatterplot : public plot{
    public:
    explicit scatterplot(plot p): plot(p){}
    std::vector<scatterPlotConfig> plottables;

    void add_plottable(scatterPlotConfig config);
    void replot();
    //void replot_plottable(int index);
};

class histogram : public plot{
public:
    explicit histogram(plot p): plot(p){}
    std::vector<histogramConfig> plottables;

    inline void add_plottable(histogramConfig config){
        config.histogram = new QCPBars(this->customPlot->xAxis, this->customPlot->yAxis);
        config.histogram->setData(config.data);
        config.histogram->setPen(Qt::NoPen);
        config.histogram->setBrush(config.brushStyle);
        this->plottables.push_back(config);
    }
    inline void replot(){
        this->customPlot->rescaleAxes();
        this->customPlot->replot();
    }
    void replot_plottable(int index);
};

class colorMap : public plot{
public:
    explicit colorMap(plot p): plot(p), plottable(){}
    colormapConfig plottable;

    void add_plottable(colormapConfig config);
    inline void replot(){
        this->plottable.colormap->rescaleDataRange();
        this->customPlot->rescaleAxes();
        this->customPlot->replot();
    }
};


class plotDisplay: public QWidget{
    Q_OBJECT

public:
    explicit plotDisplay(QWidget *parent = nullptr);
     ~plotDisplay();
    void add_plot(plot& config);
private:
    QGridLayout *layout;
    Ui::plotDisplay *ui;
    void add_labels(QCustomPlot *plot, labelContainer labels, plotFonts fonts);
    void set_theme(QCustomPlot* customPlot);
};



#endif // PLOTDISPLAY_H

