#include "livedisplay.h"
#include "ui_livedisplay.h"


liveDisplay::liveDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::liveDisplay)
{

    this->layout = new QGridLayout;
    this->setLayout(this->layout);
    ui->setupUi(this);

}

liveDisplay::liveDisplay(QMap<QString, std::shared_ptr<displayConfiguration>> displays, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::liveDisplay)
{
    this->display_register = displays;
    QGridLayout *layout= new QGridLayout;
    this->setLayout(layout);
    for (auto it = displays.begin(); it != displays.end(); it++){
        QWidget *label = new QLabel(it.value()->text_label);
        label->setObjectName(it.key());
        layout->addWidget(label, it.value()->pos.rowPos, it.value()->pos.colPos, it.value()->pos.rowSpan, it.value()->pos.colSpan);
        for (auto col_it = it.value()->LE_col.begin(); col_it != it.value()->LE_col.end(); col_it++ ){
            QWidget *LEdit = new QLineEdit(it.value()->text_label + " " + col_it.key());
            layout->addWidget(LEdit, it.value()->pos.rowPos, it.value()->pos.colPos+1);
        }
    }
    ui->setupUi(this);    
}

liveDisplay::~liveDisplay()
{
    delete ui;
}

void liveDisplay::add_display(displayConfiguration config){
    QWidget *label = new QLabel(config.text_label);
    label->setObjectName(config.text_label);
    layout->addWidget(label, config.pos.rowPos, config.pos.colPos, config.pos.rowSpan, config.pos.colSpan);
    for (auto col_it = config.LE_col.begin(); col_it != config.LE_col.end(); col_it++ ){
        std::cout << col_it.key().toStdString() << std::endl;
        QWidget *LEdit = new QLineEdit(config.text_label + " " + col_it.key());
        layout->addWidget(LEdit, config.pos.rowPos, config.pos.colPos+std::distance(config.LE_col.begin(), col_it+1));
    }
}

void liveDisplay::updateDisplay(){
    QList<QLineEdit*> LEs = this->findChildren<QLineEdit*>();
    foreach (QLineEdit *LE, LEs){
        QString name = LE->objectName();
        LE->setText(QString::number(this->display_register[name]->LE_value, this->display_register[name]->displayPrecision, this->display_register[name]->displayFormat));
    }
}
