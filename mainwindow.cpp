#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gardenwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    garden = new GardenWidget(this);
    setCentralWidget(garden);
    connect(ui->actionSave, SIGNAL(triggered()), garden, SLOT(saveForest()));
    connect(ui->actionLoad, SIGNAL(triggered()), garden, SLOT(loadForest()));
    connect(ui->actionAbout, &QAction::triggered, [this]{
        QMessageBox::about(this, "关于", "OpenGL花园 v1.1\n"
                           "by 5120379064 丁卓成 at tcbbdddd@gmail.com");
    });
    connect(ui->actionHelp, &QAction::triggered, [this]{
        QMessageBox::about(this, "帮助",
                           "一个OpenGL实现的简易花园\n"
                           "使用WASD移动，使用鼠标改变视角\n"
                           "使用I键和K键升高和降低（一旦移动将回到地面高度）\n"
                           "单击鼠标右键或P键进入种树模式\n"
                           "在种树模式下单击鼠标左键，种植一棵分形树\n"
                           "在种树模式下按N键，种植一棵简易树\n"
                           "在种树模式下再次单击鼠标右键，退出种树模式\n"
                           "在种树模式下选中一棵树时，单击鼠标左键（分形树）或按N键（简易树）修改树的参数\n"
                           "调整下雪控制窗口中的雪量和风速来控制下雪效果，雪量为0即停止下雪\n"
                           "雪可以在地面和简易树上堆积，堆积过多会产生崩塌，分形树的堆积暂未实现\n"
                           "停止下雪后已产生的积雪会逐渐融化消失\n");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
