#include "treedialog.h"
#include "ui_treedialog.h"
#include "terrain.h"
#include "gardenwidget.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const int RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 16);
const int RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2));
const int HEIGHT_MIN = int(HEIGHT / 32);
const int HEIGHT_MAX = int(HEIGHT / 8);

TreeDialog::TreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TreeDialog)
{
    ui->setupUi(this);
    tree = nullptr;

    connect(ui->base_radius_slider, SIGNAL(valueChanged(int)), ui->radius_label, SLOT(setNum(int)));
    ui->base_radius_slider->setMinimum(RADIUS_MIN);
    ui->base_radius_slider->setMaximum(RADIUS_MAX);
    ui->base_radius_slider->setValue((RADIUS_MIN + RADIUS_MAX) / 2);
    connect(ui->base_radius_slider, SIGNAL(valueChanged(int)), SLOT(on_base_radius_slider_valueChanged(int)));

    connect(ui->height_slider, SIGNAL(valueChanged(int)), ui->height_label, SLOT(setNum(int)));
    ui->height_slider->setMinimum(HEIGHT_MIN);
    ui->height_slider->setMaximum(HEIGHT_MAX);
    ui->height_slider->setValue((HEIGHT_MIN + HEIGHT_MAX) / 2);
    connect(ui->height_slider, SIGNAL(valueChanged(int)), SLOT(on_height_slider_valueChanged(int)));

    connect(ui->theta_slider, SIGNAL(valueChanged(int)), ui->theta_label, SLOT(setNum(int)));
    ui->theta_label->setNum(0);
    ui->theta_slider->setMinimum(0);
    ui->theta_slider->setMaximum(120);
    connect(ui->theta_slider, SIGNAL(valueChanged(int)), SLOT(on_theta_slider_valueChanged(int)));

    connect(ui->phi_slider, SIGNAL(valueChanged(int)), ui->phi_label, SLOT(setNum(int)));
    ui->phi_slider->setMinimum(30);
    ui->phi_slider->setMaximum(85);
    ui->phi_slider->setValue(60);
    connect(ui->phi_slider, SIGNAL(valueChanged(int)), SLOT(on_phi_slider_valueChanged(int)));

    connect(ui->leaf_Rslider, SIGNAL(valueChanged(int)), ui->leafR_label, SLOT(setNum(int)));
    connect(ui->leaf_Gslider, SIGNAL(valueChanged(int)), ui->leafG_label, SLOT(setNum(int)));
    connect(ui->leaf_Bslider, SIGNAL(valueChanged(int)), ui->leafB_label, SLOT(setNum(int)));
    ui->leaf_Rslider->setMinimum(0);
    ui->leaf_Gslider->setMinimum(0);
    ui->leaf_Bslider->setMinimum(0);
    ui->leaf_Rslider->setMaximum(255);
    ui->leaf_Gslider->setMaximum(255);
    ui->leaf_Bslider->setMaximum(255);
    ui->leaf_Rslider->setValue(25);
    ui->leaf_Gslider->setValue(175);
    ui->leaf_Bslider->setValue(25);
    connect(ui->leaf_Rslider, SIGNAL(valueChanged(int)), SLOT(on_leaf_Rslider_valueChanged(int)));
    connect(ui->leaf_Gslider, SIGNAL(valueChanged(int)), SLOT(on_leaf_Gslider_valueChanged(int)));
    connect(ui->leaf_Bslider, SIGNAL(valueChanged(int)), SLOT(on_leaf_Bslider_valueChanged(int)));

    connect(ui->leaf_size_slider, SIGNAL(valueChanged(int)), ui->leaf_size_label, SLOT(setNum(int)));
    ui->leaf_size_slider->setMinimum(15);
    ui->leaf_size_slider->setMaximum(80);
    ui->leaf_size_slider->setValue(45);
    connect(ui->leaf_size_slider, SIGNAL(valueChanged(int)), SLOT(on_leaf_size_slider_valueChanged(int)));

    connect(ui->leaf_width_slider, SIGNAL(valueChanged(int)), ui->leaf_width_label, SLOT(setNum(int)));
    ui->leaf_width_slider->setMinimum(20);
    ui->leaf_width_slider->setMaximum(150);
    ui->leaf_width_slider->setValue(50);
    connect(ui->leaf_width_slider, SIGNAL(valueChanged(int)), SLOT(on_leaf_width_slider_valueChanged(int)));
}

TreeDialog::~TreeDialog()
{
    delete ui;
}


void TreeDialog::setTree(Tree *tree, bool exist) {
    this->tree = tree;
    this->exist = exist;
    if (exist) {
        info = tree->getInfo();
        ui->base_radius_slider->setValue(info.base_radius);
        ui->height_slider->setValue(info.height);
        ui->theta_slider->setValue(info.theta);
        ui->phi_slider->setValue(info.phi);
        ui->leaf_Rslider->setValue(int(info.leafR * 255.0));
        ui->leaf_Gslider->setValue(int(info.leafG * 255.0));
        ui->leaf_Bslider->setValue(int(info.leafB * 255.0));
        ui->leaf_size_slider->setValue(info.leaf_size);
        ui->leaf_width_slider->setValue(int(info.leaf_ratio * 100.0));
    }
    else {
        ui->base_radius_slider->setValue((RADIUS_MIN + RADIUS_MAX) / 2);
        ui->height_slider->setValue((HEIGHT_MIN + HEIGHT_MAX) / 2);
        ui->theta_slider->setValue(0);
        ui->phi_slider->setValue(60);
        ui->leaf_Rslider->setValue(25);
        ui->leaf_Gslider->setValue(175);
        ui->leaf_Bslider->setValue(25);
        ui->leaf_size_slider->setValue(45);
        ui->leaf_width_slider->setValue(50);
    }
}

void TreeDialog::recover() {
    if (tree && exist)
        tree->setInfo(info);
}

bool TreeDialog::isExist() {
    return exist;
}

void TreeDialog::on_base_radius_slider_valueChanged(int value)
{
    if (tree)
        tree->setBaseRadius(value);
}

void TreeDialog::on_height_slider_valueChanged(int value)
{
    if (tree)
        tree->setHeight(value);
}

void TreeDialog::on_theta_slider_valueChanged(int value)
{
    if (tree)
        tree->setTheta(value);
}

void TreeDialog::on_phi_slider_valueChanged(int value)
{
    if (tree)
        tree->setPhi(value);
}

void TreeDialog::on_leaf_Rslider_valueChanged(int value)
{
    if (tree)
        tree->setLeafR(value);
}

void TreeDialog::on_leaf_Gslider_valueChanged(int value)
{
    if (tree)
        tree->setLeafG(value);
}

void TreeDialog::on_leaf_Bslider_valueChanged(int value)
{
    if (tree)
        tree->setLeafB(value);
}

void TreeDialog::on_leaf_size_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafSize(value);
}

void TreeDialog::on_leaf_width_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafRatio(value);
}
