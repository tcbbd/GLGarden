#ifndef TREE_H
#define TREE_H

#include <QObject>
#include <QGLWidget>
#include <QDataStream>

#include <GL/glu.h>

class Terrain;

struct TreeInfo {
    int level;
    float posX, posZ;
    float base_radius, height;
    int theta, phi;
    float leafR, leafG, leafB;
    float leaf_size, leaf_ratio;
};

QDataStream& operator << (QDataStream& out, TreeInfo info);
QDataStream& operator >> (QDataStream& in, TreeInfo &info);

class Tree : public QObject
{
    Q_OBJECT
public:
    explicit Tree(QObject *parent = 0);
    Tree(Terrain *tr, float posX = 0, float posZ = 0, QObject *parent = 0);
    ~Tree();
    void render(GLenum style);
    void draw_branch(int level, float radius, float height);
    void draw_leaves(int level, float height);
    void draw_leaf(int level);

    float getX() const;
    float getZ() const;
    TreeInfo getInfo() const;
    void setInfo(TreeInfo info);

    void setBaseRadius(float radius);
    void setHeight(float height);
    void setTheta(int degree);
    void setPhi(int degree);
    void setLeafR(int value);
    void setLeafG(int value);
    void setLeafB(int value);
    void setLeafSize(int value);
    void setLeafRatio(int value);
signals:

public slots:

private:
    GLUquadricObj *qobj;
    Terrain *terrain;
    QImage texture;
    GLuint texture_id;

    int level;
    float posX, posZ;
    float base_radius, height;
    int theta, phi;
    float leafR, leafG, leafB;
    float leaf_size, leaf_ratio;
};

#endif // TREE_H
