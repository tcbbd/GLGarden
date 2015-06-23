#include "snowdialog.h"
#include "ui_snowdialog.h"
#include "snow.h"

SnowDialog::SnowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnowDialog)
{
    ui->setupUi(this);
}

SnowDialog::SnowDialog(Snow *snow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnowDialog)
{
    ui->setupUi(this);
    this->snow = snow;

    ui->snownum_slider->setMinimum(0);
    ui->snownum_slider->setMaximum(SNOWNUM_MAX);
    ui->snownum_slider->setValue(0);
    connect(ui->snownum_slider, SIGNAL(valueChanged(int)), this, SLOT(on_snownum_slider_valueChanged(int)));

    QRegExp rx(R"(^(-?\d+)(\.\d+)?$)");
    QRegExpValidator *rxvalid = new QRegExpValidator(rx, this);
    ui->wind_xEdit->setValidator(rxvalid);
    ui->wind_yEdit->setValidator(rxvalid);
    ui->wind_zEdit->setValidator(rxvalid);
    ui->wind_xEdit->setText("0");
    ui->wind_yEdit->setText("0");
    ui->wind_zEdit->setText("0");
    connect(ui->change_wind_button, SIGNAL(pressed()), this, SLOT(on_change_wind_button_clicked()));
}

SnowDialog::~SnowDialog()
{
    delete ui;
}

void SnowDialog::on_change_wind_button_clicked()
{
    snow->setWind_x(ui->wind_xEdit->text().toFloat());
    snow->setWind_y(ui->wind_yEdit->text().toFloat());
    snow->setWind_z(ui->wind_zEdit->text().toFloat());
}

void SnowDialog::on_snownum_slider_valueChanged(int value)
{
    snow->setSnow_perframe(value);
}
