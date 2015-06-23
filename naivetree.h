#ifndef NAIVETREE_H
#define NAIVETREE_H

#include "snow.h"

#include <QObject>
#include <QGLWidget>
#include <QVector>

#include <GL/glu.h>

class Terrain;
class Snow;

struct NaiveTreeInfo {
    float posX, posZ;
    float base_radius, height, leaf_base_radius;
    float rootR, rootG, rootB;
    float leafR, leafG, leafB;
};


class NaiveTree : public QObject
{
    Q_OBJECT
public:
    explicit NaiveTree(QObject *parent = 0);
    NaiveTree(Terrain *tr, Snow *snow, float posX = 0, float posZ = 0, QObject *parent = 0);
    ~NaiveTree();
    void render(GLenum style);

    float getX() const;
    float getZ() const;
    NaiveTreeInfo getInfo() const;
    void setInfo(NaiveTreeInfo info);

    void setBaseRadius(float radius);
    void setHeight(float height);
    void setLeafRadius(float radius);
    void setRootR(int value);
    void setRootG(int value);
    void setRootB(int value);
    void setLeafR(int value);
    void setLeafG(int value);
    void setLeafB(int value);

    bool hit(Particle p, QVector<Particle> &generate);
signals:

public slots:

private:
    void draw_level(int i);

    GLUquadricObj *qobj;
    Terrain *terrain;
    Snow *snow;

    float posX, posZ;
    float base_radius, height, leaf_base_radius;
    float rootR, rootG, rootB;
    float leafR, leafG, leafB;

    int frame_count;

    QVector<Particle> particles[3];
};

#endif // NAIVETREE_H
