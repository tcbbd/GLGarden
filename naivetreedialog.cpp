#include "naivetreedialog.h"
#include "ui_naivetreedialog.h"
#include "terrain.h"
#include "gardenwidget.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const int RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 16);
const int RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 4);
const int LEAF_RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2));
const int LEAF_RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) * 3);
const int HEIGHT_MIN = int(HEIGHT / 32);
const int HEIGHT_MAX = int(HEIGHT / 8);

NaiveTreeDialog::NaiveTreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NaiveTreeDialog)
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

    connect(ui->leaf_size_slider, SIGNAL(valueChanged(int)), ui->leaf_size_label, SLOT(setNum(int)));
    ui->leaf_size_slider->setMinimum(LEAF_RADIUS_MIN);
    ui->leaf_size_slider->setMaximum(LEAF_RADIUS_MAX);
    ui->leaf_size_slider->setValue((LEAF_RADIUS_MIN + LEAF_RADIUS_MAX) / 2);
    connect(ui->leaf_size_slider, SIGNAL(valueChanged(int)), SLOT(on_leaf_size_slider_valueChanged(int)));

    connect(ui->rootR_slider, SIGNAL(valueChanged(int)), ui->rootR_label, SLOT(setNum(int)));
    connect(ui->rootG_slider, SIGNAL(valueChanged(int)), ui->rootG_label, SLOT(setNum(int)));
    connect(ui->rootB_slider, SIGNAL(valueChanged(int)), ui->rootB_label, SLOT(setNum(int)));
    ui->rootR_slider->setMinimum(0);
    ui->rootG_slider->setMinimum(0);
    ui->rootB_slider->setMinimum(0);
    ui->rootR_slider->setMaximum(255);
    ui->rootG_slider->setMaximum(255);
    ui->rootB_slider->setMaximum(255);
    ui->rootR_slider->setValue(200);
    ui->rootG_slider->setValue(150);
    ui->rootB_slider->setValue(100);
    connect(ui->rootR_slider, SIGNAL(valueChanged(int)), SLOT(on_rootR_slider_valueChanged(int)));
    connect(ui->rootG_slider, SIGNAL(valueChanged(int)), SLOT(on_rootG_slider_valueChanged(int)));
    connect(ui->rootB_slider, SIGNAL(valueChanged(int)), SLOT(on_rootB_slider_valueChanged(int)));

    connect(ui->leafR_slider, SIGNAL(valueChanged(int)), ui->leafR_label, SLOT(setNum(int)));
    connect(ui->leafG_slider, SIGNAL(valueChanged(int)), ui->leafG_label, SLOT(setNum(int)));
    connect(ui->leafB_slider, SIGNAL(valueChanged(int)), ui->leafB_label, SLOT(setNum(int)));
    ui->leafR_slider->setMinimum(0);
    ui->leafG_slider->setMinimum(0);
    ui->leafB_slider->setMinimum(0);
    ui->leafR_slider->setMaximum(255);
    ui->leafG_slider->setMaximum(255);
    ui->leafB_slider->setMaximum(255);
    ui->leafR_slider->setValue(25);
    ui->leafG_slider->setValue(175);
    ui->leafB_slider->setValue(25);
    connect(ui->leafR_slider, SIGNAL(valueChanged(int)), SLOT(on_leafR_slider_valueChanged(int)));
    connect(ui->leafG_slider, SIGNAL(valueChanged(int)), SLOT(on_leafG_slider_valueChanged(int)));
    connect(ui->leafB_slider, SIGNAL(valueChanged(int)), SLOT(on_leafB_slider_valueChanged(int)));
}

NaiveTreeDialog::~NaiveTreeDialog()
{
    delete ui;
}

void NaiveTreeDialog::setTree(NaiveTree *tree, bool exist) {
    this->tree = tree;
    this->exist = exist;
    if (exist) {
        info = tree->getInfo();
        ui->base_radius_slider->setValue(info.base_radius);
        ui->height_slider->setValue(info.height);
        ui->leaf_size_slider->setValue(info.leaf_base_radius);
        ui->rootR_slider->setValue(int(info.rootR * 255.0));
        ui->rootG_slider->setValue(int(info.rootG * 255.0));
        ui->rootB_slider->setValue(int(info.rootB * 255.0));
        ui->leafR_slider->setValue(int(info.leafR * 255.0));
        ui->leafG_slider->setValue(int(info.leafG * 255.0));
        ui->leafB_slider->setValue(int(info.leafB * 255.0));
    }
    else {
        ui->base_radius_slider->setValue((RADIUS_MIN + RADIUS_MAX) / 2);
        ui->height_slider->setValue((HEIGHT_MIN + HEIGHT_MAX) / 2);
        ui->leaf_size_slider->setValue((LEAF_RADIUS_MIN + LEAF_RADIUS_MAX) / 2);
        ui->rootR_slider->setValue(200);
        ui->rootG_slider->setValue(150);
        ui->rootB_slider->setValue(100);
        ui->leafR_slider->setValue(25);
        ui->leafG_slider->setValue(175);
        ui->leafB_slider->setValue(25);
    }
}

void NaiveTreeDialog::recover() {
    if (tree && exist)
        tree->setInfo(info);
}

bool NaiveTreeDialog::isExist() {
    return exist;
}

void NaiveTreeDialog::on_rootR_slider_valueChanged(int value)
{
    if (tree)
        tree->setRootR(value);
}

void NaiveTreeDialog::on_rootG_slider_valueChanged(int value)
{
    if (tree)
        tree->setRootG(value);
}

void NaiveTreeDialog::on_rootB_slider_valueChanged(int value)
{
    if (tree)
        tree->setRootB(value);
}

void NaiveTreeDialog::on_leafR_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafR(value);
}

void NaiveTreeDialog::on_leafG_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafG(value);
}

void NaiveTreeDialog::on_leafB_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafB(value);
}

void NaiveTreeDialog::on_base_radius_slider_valueChanged(int value)
{
    if (tree)
        tree->setBaseRadius(value);
}

void NaiveTreeDialog::on_height_slider_valueChanged(int value)
{
    if (tree)
        tree->setHeight(value);
}

void NaiveTreeDialog::on_leaf_size_slider_valueChanged(int value)
{
    if (tree)
        tree->setLeafRadius(value);
}
