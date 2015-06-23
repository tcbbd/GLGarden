#include "naivetree.h"
#include "terrain.h"
#include "snow.h"
#include "gardenwidget.h"

#include <QtCore/qmath.h>
#include <algorithm>
#include <ctime>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const float LEAF_HEIGHT_RATIO = 1.0 / 3.0;
const float LEAF_RATIO = 2.0 / 3.0;
const float LEVEL1_RATIO = (1 - LEAF_HEIGHT_RATIO) *
        (1 - LEAF_RATIO) / (1 - LEAF_RATIO * LEAF_RATIO * LEAF_RATIO);

const int RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 16);
const int RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) / 4);
const int LEAF_RADIUS_MIN = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2));
const int LEAF_RADIUS_MAX = int(MIN(WIDTH / CELL_NUM / 2, LENGTH / CELL_NUM / 2) * 3);
const int HEIGHT_MIN = int(HEIGHT / 32);
const int HEIGHT_MAX = int(HEIGHT / 8);

const float SPHERE_RATIO = 1.0 / 4.0;

const int MAX_SNOWBALL = 70;
const int MELT_FRAME_COUNT = 150;
const int MELT_SNOWBALL = 10;

const int GLU_SLICES = 20;
const int GLU_STACKS = 20;

NaiveTree::NaiveTree(QObject *parent) :
    QObject(parent)
{
}

NaiveTree::NaiveTree(Terrain *tr, Snow *snow, float posX, float posZ, QObject *parent) :
    QObject(parent)
{
    std::srand(time(NULL));
    qobj = gluNewQuadric();
    terrain = tr;
    this->snow = snow;
    this->posX = posX;
    this->posZ = posZ;
    frame_count = 0;

    base_radius = (RADIUS_MIN + RADIUS_MAX) / 2;
    height = (HEIGHT_MIN + HEIGHT_MAX) / 2;
    leaf_base_radius = (LEAF_RADIUS_MIN + LEAF_RADIUS_MAX) / 2;
    rootR = 200.0 / 255;
    rootG = 150.0 / 255;
    rootB = 100.0 / 255;
    leafR = 25.0 / 255;
    leafG = 175.0 / 255;
    leafB = 25.0 / 255;
}

NaiveTree::~NaiveTree() {
    gluDeleteQuadric(qobj);
}

bool NaiveTree::hit(Particle p, QVector<Particle> &generate) {
    p.x += WIDTH / 2 - posX;
    p.z += LENGTH / 2 - posZ;
    float r = qSqrt(p.x * p.x + p.z * p.z);
    if (r >= leaf_base_radius)
        return false;
    p.y -= terrain->org_height(posX, posZ);
    float Y = p.y;
    if ((Y -= height * LEAF_HEIGHT_RATIO) < 0)
        return false;
    if ((Y -= height * LEVEL1_RATIO) < 0) {
        if (-Y / (height * LEVEL1_RATIO) * leaf_base_radius > r) {
            p.y = height * LEVEL1_RATIO - r * height * LEVEL1_RATIO / leaf_base_radius;
            particles[0].push_back(p);

            if (particles[0].size() > MAX_SNOWBALL) {
                std::random_shuffle(particles[0].begin(), particles[0].end());
                int size = particles[0].size();
                float y = terrain->org_height(posX, posZ) + height * LEAF_HEIGHT_RATIO - 0.01;
                for (int i = size - 1; i > size / 2; --i) {
                    Particle p = particles[0][i];
                    particles[0].pop_back();
                    p.x -= WIDTH / 2 - posX;
                    p.z -= LENGTH / 2 - posZ;
                    p.y = y;
                    p.vx = p.vy = p.vz = 0;
                    generate.push_back(p);
                }
            }
            return true;
        }
        else
            return false;
    }
    if ((Y -= height * LEVEL1_RATIO * LEAF_RATIO) < 0) {
        if (-Y / (height * LEVEL1_RATIO) * leaf_base_radius > r) {
            p.y = height * LEVEL1_RATIO * LEAF_RATIO - r * height * LEVEL1_RATIO / leaf_base_radius;
            particles[1].push_back(p);

            if (particles[1].size() > int(float(MAX_SNOWBALL) * LEAF_RATIO)) {
                std::random_shuffle(particles[1].begin(), particles[1].end());
                int size = particles[1].size();
                float y = terrain->org_height(posX, posZ) + height * (LEAF_HEIGHT_RATIO + LEVEL1_RATIO) - 0.01;
                for (int i = size - 1; i > size / 2; --i) {
                    Particle p = particles[1][i];
                    particles[1].pop_back();
                    p.x -= WIDTH / 2 - posX;
                    p.z -= LENGTH / 2 - posZ;
                    p.y = y;
                    p.vx = p.vy = p.vz = 0;
                    generate.push_back(p);
                }
            }
            return true;
        }
        else
            return false;
    }
    if ((Y -= height * LEVEL1_RATIO * LEAF_RATIO * LEAF_RATIO) < 0) {
        if (-Y / (height * LEVEL1_RATIO) * leaf_base_radius > r) {
            p.y = height * LEVEL1_RATIO * LEAF_RATIO * LEAF_RATIO - r * height * LEVEL1_RATIO / leaf_base_radius;
            particles[2].push_back(p);

            if (particles[2].size() > int(float(MAX_SNOWBALL) * LEAF_RATIO * LEAF_RATIO)) {
                std::random_shuffle(particles[2].begin(), particles[2].end());
                int size = particles[2].size();
                float y = terrain->org_height(posX, posZ) + height * (1 - LEVEL1_RATIO * LEAF_RATIO * LEAF_RATIO) - 0.01;
                for (int i = size - 1; i > size / 2; --i) {
                    Particle p = particles[2][i];
                    particles[2].pop_back();
                    p.x -= WIDTH / 2 - posX;
                    p.z -= LENGTH / 2 - posZ;
                    p.y = y;
                    p.vx = p.vy = p.vz = 0;
                    generate.push_back(p);
                }
            }
            return true;
        }
        else
            return false;
    }
    return false;
}

void NaiveTree::draw_level(int i) {
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1, 1, 1);
    for (Particle &p : particles[i]) {
        glPushMatrix();
        glTranslatef(p.z, p.x, p.y + p.size * SPHERE_RATIO);
        gluSphere(qobj, p.size * SPHERE_RATIO, GLU_SLICES, GLU_STACKS);
        glPopMatrix();
    }
    glPopAttrib();
}

void NaiveTree::render(GLenum style) {
    gluQuadricDrawStyle(qobj, style);
    glPushAttrib(GL_CURRENT_BIT);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();

    glTranslatef(-WIDTH/2 + posX, terrain->org_height(posX, posZ), -LENGTH/2 + posZ);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);
    glColor3f(rootR, rootG, rootB);
    gluCylinder(qobj, base_radius, 0, height, GLU_SLICES, GLU_STACKS);

    glColor3f(leafR, leafG, leafB);
    // Level 1
    glTranslatef(0, 0, height * LEAF_HEIGHT_RATIO);
    gluCylinder(qobj, leaf_base_radius, 0, height * LEVEL1_RATIO, GLU_SLICES, GLU_STACKS);
    draw_level(0);

    // Level 2
    glTranslatef(0, 0, height * LEVEL1_RATIO);
    gluCylinder(qobj, leaf_base_radius * LEAF_RATIO, 0,
                height * LEVEL1_RATIO * LEAF_RATIO, GLU_SLICES, GLU_STACKS);
    draw_level(1);

    // Level 3
    glTranslatef(0, 0, height * LEVEL1_RATIO * LEAF_RATIO);
    gluCylinder(qobj, leaf_base_radius * LEAF_RATIO * LEAF_RATIO, 0,
                height * LEVEL1_RATIO * LEAF_RATIO * LEAF_RATIO, GLU_SLICES, GLU_STACKS);
    draw_level(2);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();

    if (snow->getSnow_perframe() == 0) {
        frame_count++;
        if (frame_count == MELT_FRAME_COUNT) {
            frame_count = 0;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < MELT_SNOWBALL; ++j) {
                    if (particles[i].empty())
                        break;
                    else
                        particles[i].pop_back();
                }
            }
        }
    }
}
float NaiveTree::getX() const
{
    return posX;
}

float NaiveTree::getZ() const
{
    return posZ;
}

void NaiveTree::setBaseRadius(float radius) {
    base_radius = radius;
    for (int i = 0; i < 3; ++i)
        particles[i].clear();
}

void NaiveTree::setHeight(float height) {
    this->height = height;
    for (int i = 0; i < 3; ++i)
        particles[i].clear();
}

void NaiveTree::setLeafRadius(float radius) {
    leaf_base_radius = radius;
    for (int i = 0; i < 3; ++i)
        particles[i].clear();
}

void NaiveTree::setRootR(int value) {
    rootR = value / 255.0;
}

void NaiveTree::setRootG(int value) {
    rootG = value / 255.0;
}

void NaiveTree::setRootB(int value) {
    rootB = value / 255.0;
}

void NaiveTree::setLeafR(int value) {
    leafR = value / 255.0;
}

void NaiveTree::setLeafG(int value) {
    leafG = value / 255.0;
}

void NaiveTree::setLeafB(int value) {
    leafB = value / 255.0;
}

NaiveTreeInfo NaiveTree::getInfo() const {
    NaiveTreeInfo ret;
    ret.base_radius = base_radius;
    ret.height = height;
    ret.leaf_base_radius = leaf_base_radius;
    ret.posX = posX;
    ret.posZ = posZ;
    ret.rootR = rootR;
    ret.rootG = rootG;
    ret.rootB = rootB;
    ret.leafR = leafR;
    ret.leafG = leafG;
    ret.leafB = leafB;

    return ret;
}

void NaiveTree::setInfo(NaiveTreeInfo info) {
    base_radius = info.base_radius;
    height = info.height;
    leaf_base_radius = info.leaf_base_radius;
    posX = info.posX;
    posZ = info.posZ;
    rootR = info.rootR;
    rootG = info.rootG;
    rootB = info.rootB;
    leafR = info.leafR;
    leafG = info.leafG;
    leafB = info.leafB;
}
