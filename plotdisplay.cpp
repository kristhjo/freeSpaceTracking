#include "plotdisplay.h"
#include "ui_plotdisplay.h"


plotDisplay::plotDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotDisplay)
{
    this->layout = new QGridLayout;
    this->setLayout(layout);
    ui->setupUi(this);
}

plotDisplay::~plotDisplay()
{
    std::cout << "plotDisplay destructor called"<< std::endl;
    delete ui;
    std::cout << "succesfully delete plotDisplay" << std::endl;
}

void plotDisplay::add_plot(plot& config){
    config.customPlot = new QCustomPlot();
    config.customPlot->setObjectName(config.plotName);

    this->layout->addWidget(config.customPlot, config.pos.rowPos, config.pos.colPos, config.pos.rowSpan, config.pos.colSpan);
    add_labels(config.customPlot, config.labels, config.fontStyles);
    this->set_theme(config.customPlot);

    config.customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
    config.customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    config.customPlot->legend->setBorderPen(Qt::NoPen);
    config.customPlot->legend->setFont(config.fontStyles.legendFont);
    config.customPlot->legend->setVisible(false);
}

void plotDisplay::add_labels(QCustomPlot *plot, labelContainer labels, plotFonts fonts){
    if(labels.xAxisLabel != ""){
        plot->xAxis->setLabel(labels.xAxisLabel);
        plot->xAxis->setLabelFont(fonts.axisFont);
        plot->xAxis->setLabelColor(Qt::white);
    }
    if(labels.yAxisLabel != ""){
        plot->yAxis->setLabel(labels.yAxisLabel);
        plot->yAxis->setLabelFont(fonts.axisFont);
        plot->yAxis->setLabelColor(Qt::white);
    }
    if(labels.title!=""){
        plot->plotLayout()->insertRow(0);
        plot->plotLayout()->addElement(0,0, new QCPTextElement(plot, labels.title, fonts.titleFont));
        qobject_cast<QCPTextElement*>(plot->plotLayout()->element(0,0))->setTextColor(Qt::white);

    }
    if(labels.xAxis2Label != ""){
        plot->xAxis2->setLabel(labels.xAxis2Label);
        plot->xAxis2->setLabelFont(fonts.axisFont);
        plot->xAxis2->setLabelColor(Qt::white);
    }
    if(labels.yAxis2Label != ""){
        plot->yAxis2->setLabel(labels.yAxis2Label);
        plot->yAxis2->setLabelFont(fonts.axisFont);
        plot->yAxis2->setLabelColor(Qt::white);
    }
}

void plotDisplay::set_theme(QCustomPlot* customPlot){
    ///taken from QCustomplot theme demo
    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    customPlot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    customPlot->axisRect()->setBackground(axisRectGradient);
}

void scatterplot::add_plottable(scatterPlotConfig config){
    if(this->customPlot != nullptr){
       config.scatterPlot = this->customPlot->addGraph(this->customPlot->axisRect()->axis(config.xaxes), this->customPlot->axisRect()->axis(config.yaxes));
       config.scatterPlot->setData(config.data);
       config.scatterPlot->setLineStyle(config.linestyle);
       config.scatterPlot->setScatterStyle(config.scatterStyle);
       config.dateTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
       config.dateTicker->setDateTimeFormat("hh:mm:ss");
       this->customPlot->axisRect()->axis(config.xaxes)->setTicker(config.dateTicker);
       this->customPlot->xAxis->setTicker(config.dateTicker);
       if (config.legendText!=""){
           config.scatterPlot->setName(config.legendText);
           customPlot->legend->setVisible(true);
       }
       this->customPlot->yAxis->setRange(QCPRange(0.9,1.1));
       this->customPlot->replot();
       this->plottables.push_back(config);
    }
}
void scatterplot::replot(){
    if(this->customPlot){
        bool sensibleRange;
        for(auto it = this->plottables.begin(); it!= this->plottables.end(); it++){
            QCPRange yRange = it->data->valueRange(sensibleRange);
            QCPRange plotRange = this->customPlot->axisRect()->axis(it->yaxes)->range();
            if(sensibleRange){
                if(yRange.upper*it->yAxisHighRangeFactor > plotRange.upper){
                    plotRange.upper = yRange.upper*it->yAxisHighRangeFactor;
                }
                if(yRange.lower*it->yAxisLowRangeFactor < plotRange.lower){
                    plotRange.lower = yRange.lower*it->yAxisLowRangeFactor;
                }
                this->customPlot->axisRect()->axis(it->yaxes)->setRange(plotRange);
                this->customPlot->xAxis->rescale();
            }
            else {
                this->customPlot->rescaleAxes();
            }
        }
        this->customPlot->replot();
    }
}

void colorMap::add_plottable(colormapConfig config){
    this->plottable = config;
    this->plottable.colormap = new QCPColorMap(this->customPlot->xAxis, this->customPlot->yAxis);
    this->plottable.colorscale = new QCPColorScale(this->customPlot);
    this->customPlot->plotLayout()->addElement(1,1,this->plottable.colorscale);
    this->plottable.colorscale->setType(QCPAxis::atLeft); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    this->plottable.colormap->setColorScale(this->plottable.colorscale); // associate the color map with the color scale
    this->plottable.colormap->setGradient(QCPColorGradient::gpNight);
    this->plottable.colorscale->setDataRange(QCPRange(0,1));
    QCPMarginGroup* marginGroup = new QCPMarginGroup(this->customPlot);
    this->customPlot->axisRect(0)->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    this->plottable.colorscale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    this->customPlot->rescaleAxes();
    this->customPlot->replot();
}


/*
            if(config->save_settings.save_plot){
                if(config->save_settings.type_name == "png"){
                    plot->savePng(config->save_settings.file_name, config->save_settings.x_dim, config->save_settings.y_dim);

}*/
