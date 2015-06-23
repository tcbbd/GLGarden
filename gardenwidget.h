#ifndef GARDENWIDGET_H
#define GARDENWIDGET_H

#include "skybox.h"
#include "camera.h"
#include "terrain.h"
#include "tree.h"
#include "snow.h"
#include "snowdialog.h"
#include "naivetree.h"
#include "naivetreedialog.h"

#include <QGLWidget>
#include <QVector>
#include <QFile>
#include <QTimer>

class TreeDialog;

const float WIDTH = 8192;
const float HEIGHT = 8192;
const float LENGTH = 8192;
const int INTERVAL = 1000 / 120;

class GardenWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GardenWidget(QWidget *parent = 0);
    ~GardenWidget();

signals:

public slots:
    void saveForest();
    void loadForest();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    SkyBox skybox;
    Camera camera;
    Terrain terrain;
    QVector<Tree*> forest;
    QVector<NaiveTree*> naiveforest;
    Snow snow;
    QTimer *timer;

    TreeDialog *tree_dialog;
    NaiveTreeDialog *naivetree_dialog;
    SnowDialog *snow_dialog;
    bool press_enable;
};

#endif // GARDENWIDGET_H
