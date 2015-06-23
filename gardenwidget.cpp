#include "gardenwidget.h"
#include "treedialog.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QDataStream>

#include <GL/glu.h>
#include <ctime>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const float DIST = MIN(WIDTH / CELL_NUM, LENGTH / CELL_NUM);

GardenWidget::GardenWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setMinimumSize(800, 600);
    setFocusPolicy(Qt::ClickFocus);
    qsrand(time(NULL));
    timer = new QTimer();
    timer->setInterval(INTERVAL);
    timer->setTimerType(Qt::CoarseTimer);
    connect(timer, SIGNAL(timeout()), SLOT(updateGL()));

    tree_dialog = new TreeDialog(this);
    press_enable = true;
    connect(tree_dialog, &TreeDialog::accepted, [this]{press_enable = true;});
    connect(tree_dialog, &TreeDialog::rejected, [this]{
        if (!tree_dialog->isExist()) {
            delete forest.back();
            forest.pop_back();
        }
        else {
            tree_dialog->recover();
        }
        press_enable = true;
    });

    naivetree_dialog = new NaiveTreeDialog(this);
    connect(naivetree_dialog, &NaiveTreeDialog::accepted, [this]{press_enable = true;});
    connect(naivetree_dialog, &NaiveTreeDialog::rejected, [this] {
       if (!naivetree_dialog->isExist()) {
           delete naiveforest.back();
           naiveforest.pop_back();
       }
       else {
           naivetree_dialog->recover();
       }
       press_enable = true;
    });

    snow_dialog = new SnowDialog(&snow, this);
}

GardenWidget::~GardenWidget() {
    for (Tree *t : forest)
        delete t;
}

void GardenWidget::initializeGL() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    skybox.init();
    terrain.init();
    camera.init(&terrain);
    loadForest();
    snow.init(&terrain, &camera, &naiveforest);
    timer->start();
    snow_dialog->show();
}

void GardenWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (GLdouble)w/(GLdouble)h, 0.1, 15000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.setCamera();
}

void GardenWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    skybox.render();
    terrain.render();
    for (Tree *t : forest)
        t->render(GL_FILL);
    for (NaiveTree *t : naiveforest)
        t->render(GL_FILL);
    snow.render();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.setCamera();
}

void GardenWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        camera.move(Camera::FORWARD);
        break;
    case Qt::Key_S:
        camera.move(Camera::BACKWARD);
        break;
    case Qt::Key_A:
        camera.move(Camera::LEFT);
        break;
    case Qt::Key_D:
        camera.move(Camera::RIGHT);
        break;
    case Qt::Key_I:
        camera.move(Camera::UP);
        break;
    case Qt::Key_K:
        camera.move(Camera::DOWN);
        break;
    case Qt::Key_P:
        camera.toggle_select_terrain();
        break;
    case Qt::Key_N:
    {
        if (!terrain.plantable())
            return;
        float x = terrain.getHighlightX();
        float z = terrain.getHighlightZ();
        for (Tree *t : forest)
            if (t->getX() == x && t->getZ() == z)
                return;
        for (NaiveTree *t : naiveforest)
            if (-DIST <= (t->getX() - x) && (t->getX() - x) <= DIST &&
                    -DIST <= (t->getZ() - z) && (t->getZ() - z) <= DIST) {
                if (t->getX() == x && t->getZ() == z) {
                    press_enable = false;
                    naivetree_dialog->setTree(t, true);
                    naivetree_dialog->show();
                    clearFocus();
                }
                return;
            }
        NaiveTree *tree = new NaiveTree(&terrain, &snow, x, z);
        naiveforest.push_back(tree);
        press_enable = false;
        naivetree_dialog->setTree(tree, false);
        naivetree_dialog->show();
        clearFocus();
        break;
    }
    case Qt::Key_Escape:
        setCursor(Qt::ArrowCursor);
        setMouseTracking(false);
        clearFocus();
        break;
    default:
        QGLWidget::keyPressEvent(event);
        return;
    }
    //updateGL();
}

void GardenWidget::mouseMoveEvent(QMouseEvent *event) {
    int X = GetSystemMetrics(SM_CXSCREEN) >> 1;
    int Y = GetSystemMetrics(SM_CYSCREEN) >> 1;

    camera.look(Camera::RIGHT, float(event->globalX() - X) / float(width()));
    camera.look(Camera::DOWN, float(event->globalY() - Y) / float(height()));
    SetCursorPos(X, Y);
    //updateGL();
}

void GardenWidget::mousePressEvent(QMouseEvent *event) {
    if (!press_enable)
        return;
    if (event->button() == Qt::LeftButton) {
        if (!terrain.plantable())
            return;
        float x = terrain.getHighlightX();
        float z = terrain.getHighlightZ();
        for (NaiveTree *t : naiveforest)
            if (-DIST <= (t->getX() - x) && (t->getX() - x) <= DIST &&
                    -DIST <= (t->getZ() - z) && (t->getZ() - z) <= DIST) {
                return;
            }
        for (Tree *t : forest)
            if (t->getX() == x && t->getZ() == z) {
                press_enable = false;
                tree_dialog->setTree(t, true);
                tree_dialog->show();
                clearFocus();
                return;
            }
        Tree *tree = new Tree(&terrain, x, z);
        forest.push_back(tree);
        press_enable = false;
        tree_dialog->setTree(tree, false);
        tree_dialog->show();
        clearFocus();
    }
    if (event->button() == Qt::RightButton) {
        camera.toggle_select_terrain();
        //updateGL();
    }
}

void GardenWidget::focusInEvent(QFocusEvent *event) {
    int X = GetSystemMetrics(SM_CXSCREEN) >> 1;
    int Y = GetSystemMetrics(SM_CYSCREEN) >> 1;

    if (event->gotFocus()) {
        setCursor(Qt::BlankCursor);
        setMouseTracking(true);
        SetCursorPos(X, Y);
    }
}

void GardenWidget::saveForest() {
    QFile save_file("data/forest.raw");
    save_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&save_file);
    for (Tree *t : forest)
        out << t->getInfo();
}

void GardenWidget::loadForest() {
    QFile save_file("data/forest.raw");
    save_file.open(QIODevice::ReadOnly);
    if (!save_file.exists())
        return;
    QDataStream in(&save_file);
    TreeInfo info;
    for (Tree *t : forest)
        delete t;
    forest.clear();
    while (!in.atEnd()) {
        in >> info;
        Tree *t = new Tree(&terrain);
        t->setInfo(info);
        forest.push_back(t);
    }
}
