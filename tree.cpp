#include "tree.h"
#include "terrain.h"
#include "gardenwidget.h"
#include "treedialog.h"

#include <QtCore/qmath.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const float CHILD_RATIO = 1.0 / 3.0;
const float SIDE_BRANCH_HEIGHT_RATIO = 1.0 / 2.0;
const float SIDE_BRANCH_LENGTH_RATIO = 0.7;

const int RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 16);
const int RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2));
const int HEIGHT_MIN = int(HEIGHT / 32);
const int HEIGHT_MAX = int(HEIGHT / 8);

const int GLU_SLICES = 20;
const int GLU_STACKS = 20;

Tree::Tree(QObject *parent) :
    QObject(parent)
{
}

Tree::Tree(Terrain *tr, float posX, float posZ, QObject *parent) :
    QObject(parent)
{
    texture.load("data/tree.bmp", "BMP");
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    texture = QGLWidget::convertToGLFormat(texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(),
                      0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    qobj = gluNewQuadric();
    terrain = tr;

    this->posX = posX;
    this->posZ = posZ;

    level = 4;
    base_radius = (RADIUS_MIN + RADIUS_MAX) / 2;
    height = (HEIGHT_MIN + HEIGHT_MAX) / 2;
    theta = 0;
    phi = 60;
    leafR = 25.0 / 255;
    leafG = 175.0 / 255;
    leafB = 25.0 / 255;
    leaf_size = 45;
    leaf_ratio = 0.5;
}

Tree::~Tree() {
    gluDeleteQuadric(qobj);
    glDeleteTextures(1, &texture_id);
}

void Tree::draw_leaf(int level) {
    float size = leaf_size;
    for (int i = level; i < this->level; ++i)
        size *= 0.8;
    float angle_ratio = 1;

    glPushAttrib(GL_CURRENT_BIT);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(leafR, leafG, leafB);
    glVertex3f(0, 0, 0);
    glVertex3f(0, -size * leaf_ratio / 2, size * leaf_ratio * angle_ratio);
    glVertex3f(0, size * leaf_ratio / 2, size * leaf_ratio * angle_ratio);
    glVertex3f(0, -size * leaf_ratio / 2, size * leaf_ratio * angle_ratio + size);
    glVertex3f(0, size * leaf_ratio / 2, size * leaf_ratio * angle_ratio + size);
    glVertex3f(0, 0, size * leaf_ratio * angle_ratio * 2 + size);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();
}

void Tree::draw_leaves(int level, float height) {
    const int LEVEL = 2;

    for (int i = 0; i < LEVEL; ++i) {
        float h = height * (i + 1) / (LEVEL + 1);
        glPushMatrix();
        glTranslatef(0, 0, h);
        glRotatef(-phi, 0, 1, 0);
        draw_leaf(level);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, h);
        glRotatef(120, 0, 0, 1);
        glRotatef(-phi, 0, 1, 0);
        draw_leaf(level);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, h);
        glRotatef(240, 0, 0, 1);
        glRotatef(-phi, 0, 1, 0);
        draw_leaf(level);
        glPopMatrix();
    }
}

void Tree::draw_branch(int level, float radius, float height) {
    if (level == 0)
        return;

    glPushMatrix();

    gluCylinder(qobj, radius, radius * CHILD_RATIO, height * (1 - CHILD_RATIO), GLU_SLICES, GLU_STACKS);

        //TOP
        glPushMatrix();
        glTranslatef(0, 0, height * (1 - CHILD_RATIO));
        draw_branch(level - 1, radius * CHILD_RATIO, height * CHILD_RATIO);
        draw_leaves(level, height * CHILD_RATIO);
        glPopMatrix();

        //SIDE
        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        draw_leaves(level, height * CHILD_RATIO);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta + 120, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        draw_leaves(level, height * CHILD_RATIO);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta + 240, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        draw_leaves(level, height * CHILD_RATIO);
        glPopMatrix();

    glPopMatrix();
}

void Tree::render(GLenum style) {
    gluQuadricDrawStyle(qobj, style);
    gluQuadricTexture(qobj, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glPushMatrix();

    glTranslatef(-WIDTH/2 + posX, terrain->org_height(posX, posZ), -LENGTH/2 + posZ);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);
    gluCylinder(qobj, base_radius, base_radius * CHILD_RATIO, height * (1 - CHILD_RATIO),
                GLU_SLICES, GLU_STACKS);

        //TOP
        glPushMatrix();
        glTranslatef(0, 0, height * (1 - CHILD_RATIO));
        draw_branch(level - 1, base_radius * CHILD_RATIO, height * CHILD_RATIO);
        glPopMatrix();

        //SIDE
        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, base_radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta + 120, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, base_radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, height * SIDE_BRANCH_HEIGHT_RATIO);
        glRotatef(theta + 240, 0, 0, 1);
        glRotatef(-phi, 1, 0, 0);
        draw_branch(level - 1, base_radius * CHILD_RATIO, height * SIDE_BRANCH_LENGTH_RATIO);
        glPopMatrix();

    glPopMatrix();
}

float Tree::getX() const {
    return posX;
}

float Tree::getZ() const {
    return posZ;
}

void Tree::setBaseRadius(float radius) {
    base_radius = radius;
}

void Tree::setHeight(float height) {
    this->height = height;
}

void Tree::setTheta(int degree) {
    theta = degree;
}

void Tree::setPhi(int degree) {
    phi = degree;
}

void Tree::setLeafR(int value) {
    leafR = value / 255.0;
}
void Tree::setLeafG(int value) {
    leafG = value / 255.0;
}
void Tree::setLeafB(int value) {
    leafB = value / 255.0;
}

void Tree::setLeafSize(int value) {
    leaf_size = value;
}

void Tree::setLeafRatio(int value) {
    leaf_ratio = value / 100.0;
}

TreeInfo Tree::getInfo() const {
    TreeInfo ret;
    ret.level = level;
    ret.base_radius = base_radius;
    ret.height = height;
    ret.posX = posX;
    ret.posZ = posZ;
    ret.theta = theta;
    ret.phi = phi;
    ret.leafR = leafR;
    ret.leafG = leafG;
    ret.leafB = leafB;
    ret.leaf_size = leaf_size;
    ret.leaf_ratio = leaf_ratio;

    return ret;
}

void Tree::setInfo(TreeInfo info) {
    level = info.level;
    base_radius = info.base_radius;
    height = info.height;
    posX = info.posX;
    posZ = info.posZ;
    theta = info.theta;
    phi = info.phi;
    leafR = info.leafR;
    leafG = info.leafG;
    leafB = info.leafB;
    leaf_size = info.leaf_size;
    leaf_ratio = info.leaf_ratio;
}

QDataStream& operator << (QDataStream& out, TreeInfo info) {
    out << info.level;
    out << info.base_radius;
    out << info.height;
    out << info.posX;
    out << info.posZ;
    out << info.theta;
    out << info.phi;
    out << info.leafR;
    out << info.leafG;
    out << info.leafB;
    out << info.leaf_size;
    out << info.leaf_ratio;
    return out;
}

QDataStream& operator >> (QDataStream& in, TreeInfo &info) {
    in >> info.level;
    in >> info.base_radius;
    in >> info.height;
    in >> info.posX;
    in >> info.posZ;
    in >> info.theta;
    in >> info.phi;
    in >> info.leafR;
    in >> info.leafG;
    in >> info.leafB;
    in >> info.leaf_size;
    in >> info.leaf_ratio;
    return in;
}
