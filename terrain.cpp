#include "terrain.h"
#include "gardenwidget.h"

#include <ctime>
#include <QImage>
#include <QtCore/qmath.h>
#include <GL/glu.h>
#include <GL/gl.h>

static const float MAX_HEIGHT = HEIGHT / 16;
static const float DELTA_HEIGHT = MAX_HEIGHT / 2;
static const int SUB_BLOCK_NUM = 8;
static const int PER_BLOCK_NUM = CELL_NUM / SUB_BLOCK_NUM;

static const int MELT_FRAME_COUNT = 150;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void Vector3::normalize() {
    float r = qSqrt(x * x + y * y + z * z);
    if (r == 0)
        return;
    x = x / r;
    y = y / r;
    z = z / r;
}

Vector3 cross_product(Vector3 &left, Vector3 &right) {
    Vector3 result;
    result.x = left.y * right.z - left.z * right.y;
    result.y = left.z * right.x - left.x * right.z;
    result.z = left.x * right.y - left.y * right.x;
    return result;
}

Cell::Cell() {
    hit_num = 0;
    snow_height = 0;
}

Terrain::Terrain(QObject *parent) :
    QObject(parent)
{
    texture[0].load("data/grass.bmp", "BMP");
    texture[1].load("data/snow.bmp", "BMP");
    texture[2].load("data/snowball.bmp", "BMP");
    highlightX = highlightZ = -1;
    snowing = false;
    frame_count = 0;

    //for (int i = 0; i <= CELL_NUM; i += PER_BLOCK_NUM)
    //    for (int j = 0; j <= CELL_NUM; j += PER_BLOCK_NUM)
    //        height_map[i][j] = (float(qrand()) / RAND_MAX) * MAX_HEIGHT;

    org_height_map[0][0] = (float(qrand()) / RAND_MAX) * MAX_HEIGHT;
    for (int j = PER_BLOCK_NUM; j <= CELL_NUM; j += PER_BLOCK_NUM) {
        org_height_map[0][j] = org_height_map[0][j - PER_BLOCK_NUM] - DELTA_HEIGHT +
                (float(qrand()) / RAND_MAX) * 2 * DELTA_HEIGHT;
        if (org_height_map[0][j] < 0 || org_height_map[0][j] > MAX_HEIGHT)
            j -= PER_BLOCK_NUM;
    }
    for (int i = PER_BLOCK_NUM; i <= CELL_NUM; i += PER_BLOCK_NUM) {
        do {
            org_height_map[i][0] = org_height_map[i - PER_BLOCK_NUM][0] - DELTA_HEIGHT +
                (float(qrand()) / RAND_MAX) * 2 * DELTA_HEIGHT;
        } while(org_height_map[i][0] < 0 || org_height_map[i][0] > MAX_HEIGHT);
        for (int j = PER_BLOCK_NUM; j <= CELL_NUM; j += PER_BLOCK_NUM) {
            float upper = MIN(org_height_map[i - PER_BLOCK_NUM][j],
                    org_height_map[i][j - PER_BLOCK_NUM]) + DELTA_HEIGHT;
            if (upper > MAX_HEIGHT)
                upper = MAX_HEIGHT;
            float lower = MAX(org_height_map[i - PER_BLOCK_NUM][j],
                    org_height_map[i][j - PER_BLOCK_NUM]) - DELTA_HEIGHT;
            if (lower < 0)
                lower = 0;
            org_height_map[i][j] = lower + (float(qrand()) / RAND_MAX) * (upper - lower);
        }
    }

    for (int i = 0; i < SUB_BLOCK_NUM; ++i)
        for (int j = 0; j < SUB_BLOCK_NUM; ++j)
            for (int x = 0; x <= PER_BLOCK_NUM; ++x)
                for (int z = 0; z <= PER_BLOCK_NUM; ++z) {
                    if (x == 0 && (z == 0 || z == PER_BLOCK_NUM))
                        continue;
                    if (x == PER_BLOCK_NUM && (z == 0 || z == PER_BLOCK_NUM))
                        continue;

                    //用双线性插值求高度
                    float X = float(x) / PER_BLOCK_NUM;
                    float Z = float(z) / PER_BLOCK_NUM;
                    float base = org_height_map[i * PER_BLOCK_NUM][j * PER_BLOCK_NUM] * (1 - X) * (1 - Z) +
                            org_height_map[(i + 1) * PER_BLOCK_NUM][j * PER_BLOCK_NUM] * (1 - X) * Z +
                            org_height_map[i * PER_BLOCK_NUM][(j + 1) * PER_BLOCK_NUM] * X * (1 - Z) +
                            org_height_map[(i + 1) * PER_BLOCK_NUM][(j + 1) * PER_BLOCK_NUM] * X * Z;
                    org_height_map[i * PER_BLOCK_NUM + z][j * PER_BLOCK_NUM + x] = base;

                    //float upper = MIN(MAX_HEIGHT, base + DELTA_HEIGHT);
                    //float lower = MAX(0, base - DELTA_HEIGHT);
                    //height_map[i * PER_BLOCK_NUM + z][j * PER_BLOCK_NUM + x] =
                    //        lower + (float(qrand()) / RAND_MAX) * (upper - lower);
                }


    /*height_map[0][0] = (float(qrand()) / RAND_MAX) * MAX_HEIGHT;
    for (int j = 1; j <= CELL_NUM; ++j) {
        height_map[0][j] = height_map[0][j - 1] - DELTA_HEIGHT +
                (float(qrand()) / RAND_MAX) * 2 * DELTA_HEIGHT;
        if (height_map[0][j] < 0 || height_map[0][j] > MAX_HEIGHT)
            --j;
    }
    for (int i = 1; i <= CELL_NUM; ++i) {
        do {
            height_map[i][0] = height_map[i - 1][0] - DELTA_HEIGHT +
                (float(qrand()) / RAND_MAX) * 2 * DELTA_HEIGHT;
        } while(height_map[i][0] < 0 || height_map[i][0] > MAX_HEIGHT);
        for (int j = 1; j <= CELL_NUM; ++j) {
            float upper = MIN(height_map[i - 1][j], height_map[i][j - 1]) + DELTA_HEIGHT;
            if (upper > MAX_HEIGHT)
                upper = MAX_HEIGHT;
            float lower = MAX(height_map[i - 1][j], height_map[i][j - 1]) - DELTA_HEIGHT;
            if (lower < 0)
                lower = 0;
            height_map[i][j] = lower + (float(qrand()) / RAND_MAX) * (upper - lower);
        }
    }*/

    for (int i = 0; i <= CELL_NUM; ++i)
        for (int j = 0; j <= CELL_NUM; ++j)
            snowed_height_map[i][j] = org_height_map[i][j];
}

Terrain::~Terrain() {
    glDeleteTextures(2, texture_id);
}

void Terrain::init() {
    glGenTextures(3, texture_id);
    for (int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, texture_id[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        texture[i] = QGLWidget::convertToGLFormat(texture[i]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture[i].width(), texture[i].height(),
                          GL_RGBA, GL_UNSIGNED_BYTE, texture[i].bits());
    }
}

void Terrain::disable_highlight() {
    highlightX = highlightZ = -1;
}

void Terrain::highlight(float eyeX, float eyeY, float eyeZ, float theta, float phi) {
    float c = qCos(theta);
    float s = qSin(theta);
    int i = int(eyeZ / (LENGTH / CELL_NUM));
    int j = int(eyeX / (WIDTH / CELL_NUM));

    float curX = eyeX, curY = eyeY, curZ = eyeZ;
    float nextX, nextY, nextZ;

    if (s > 0) {
        // x轴负向
        while (j >= 0 && i >= 0 && i < CELL_NUM) {
            nextX = j * (WIDTH / CELL_NUM);
            nextZ = curZ + (nextX - curX) * c / s;
            int ii = int(nextZ / (LENGTH / CELL_NUM));
            if (ii == i) {
                nextY = curY - qSqrt((nextX - curX) * (nextX - curX) + (nextZ - curZ) * (nextZ - curZ)) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                --j;
            }
            else {
                if (ii > i)
                    nextZ = (i + 1) * (LENGTH / CELL_NUM);
                else
                    nextZ = i * (LENGTH / CELL_NUM);
                nextX = curX + (nextZ - curZ) * s / c;
                nextY = curY - qSqrt((nextX - curX) * (nextX - curX) + (nextZ - curZ) * (nextZ - curZ)) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                if (ii > i)
                    ++i;
                else
                    --i;
            }
            curX = nextX;
            curY = nextY;
            curZ = nextZ;
        }
    }
    if (s < 0) {
        // x轴正向
        while (j < CELL_NUM && i >= 0 && i < CELL_NUM) {
            nextX = (j + 1) * (WIDTH / CELL_NUM);
            nextZ = curZ + (nextX - curX) * c / s;
            int ii = int(nextZ / (LENGTH / CELL_NUM));
            if (ii == i) {
                nextY = curY - qSqrt((nextX - curX) * (nextX - curX) + (nextZ - curZ) * (nextZ - curZ)) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                ++j;
            }
            else {
                if (ii > i)
                    nextZ = (i + 1) * (LENGTH / CELL_NUM);
                else
                    nextZ = i * (LENGTH / CELL_NUM);
                nextX = curX + (nextZ - curZ) * s / c;
                nextY = curY - qSqrt((nextX - curX) * (nextX - curX) + (nextZ - curZ) * (nextZ - curZ)) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                if (ii > i)
                    ++i;
                else
                    --i;
            }
            curX = nextX;
            curY = nextY;
            curZ = nextZ;
        }
    }
    if (s == 0) {
        // x轴方向不移动
        nextX = curX;
        if (c > 0) {
            while (i >= 0) {
                nextZ = i * (LENGTH / CELL_NUM);
                nextY = curY - (nextZ - curZ) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                curY = nextY;
                curZ = nextZ;
                --i;
            }
        }
        if (c < 0) {
            while (i < CELL_NUM) {
                nextZ = (i + 1) * (LENGTH / CELL_NUM);
                nextY = curY - (nextZ - curZ) * qTan(phi);
                if ((nextY - snowed_height(nextX, nextZ)) * (curY - snowed_height(curX, curZ)) <= 0) {
                    highlightX = j;
                    highlightZ = i;
                    return;
                }
                curY = nextY;
                curZ = nextZ;
                ++i;
            }
        }
    }

    // can't find highlighted cell
    highlightX = highlightZ = -1;
}

void Terrain::render() {
    glPushMatrix();
    glTranslatef(-WIDTH/2, 0, -LENGTH/2);

    draw_snow();

    if (highlightZ >= 0 && highlightX >= 0) {

    glPushAttrib(GL_CURRENT_BIT);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    glColor3f(0, 1, 0);
    if (highlightZ % 2 == 0) {
        // from left to right
        // ---
        // |/|
        // ---
        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX + 1], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX], (highlightZ + 1) * (LENGTH / CELL_NUM));

        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX + 1], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX], (highlightZ + 1) * (LENGTH / CELL_NUM));
        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX + 1], (highlightZ + 1) * (LENGTH / CELL_NUM));
    }
    else {
        // from right to left
        // ---
        // |\|
        // ---
        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX + 1], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX + 1], (highlightZ + 1) * (LENGTH / CELL_NUM));

        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ][highlightX], highlightZ * (LENGTH / CELL_NUM));
        glVertex3f(highlightX * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX], (highlightZ + 1) * (LENGTH / CELL_NUM));
        glVertex3f((highlightX + 1) * (WIDTH / CELL_NUM), snowed_height_map[highlightZ + 1][highlightX + 1], (highlightZ + 1) * (LENGTH / CELL_NUM));
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();

    }

    glPopMatrix();

    if (!snowing) {
        frame_count++;
        if (frame_count == MELT_FRAME_COUNT) {
            frame_count = 0;
            for (int i = 0; i < CELL_NUM; ++i)
                for (int j = 0; j < CELL_NUM; ++j) {
                    if (cell_map[i][j].hit_num > 0 && cell_map[i][j].hit_num < HIT_LIMIT)
                        cell_map[i][j].hit_num--;
                    else if (cell_map[i][j].hit_num == 0)
                        continue;
                    else {
                        if (cell_map[i][j].snow_height > 0) {
                            cell_map[i][j].snow_height -= SNOW_HEIGHT;
                            snowed_height_map[i][j] -= SNOW_HEIGHT;
                            snowed_height_map[i][j + 1] -= SNOW_HEIGHT;
                            snowed_height_map[i + 1][j] -= SNOW_HEIGHT;
                            snowed_height_map[i + 1][j + 1] -= SNOW_HEIGHT;
                        }
                        else
                            cell_map[i][j].hit_num--;
                    }
                }
        }
    }
}

void Terrain::draw_snow() {
    for (int i = 0; i < CELL_NUM; ++i)
        for (int j = 0; j < CELL_NUM; ++j) {
            if (cell_map[i][j].hit_num < HIT_LIMIT)
                glBindTexture(GL_TEXTURE_2D, texture_id[0]);
            else
                glBindTexture(GL_TEXTURE_2D, texture_id[1]);
            glBegin(GL_TRIANGLE_STRIP);
            if (i % 2 == 0) {
                // from left to right
                // ---
                // |/|
                // ---
                glTexCoord2d(0, 1); glVertex3f(j * (WIDTH / CELL_NUM), snowed_height_map[i][j], i * (LENGTH / CELL_NUM));
                glTexCoord2d(0, 0); glVertex3f(j * (WIDTH / CELL_NUM), snowed_height_map[i + 1][j], (i + 1) * (LENGTH / CELL_NUM));
                glTexCoord2d(1, 1); glVertex3f((j + 1) * (WIDTH / CELL_NUM), snowed_height_map[i][j + 1], i * (LENGTH / CELL_NUM));
                glTexCoord2d(1, 0); glVertex3f((j + 1) * (WIDTH / CELL_NUM), snowed_height_map[i + 1][j + 1], (i + 1) * (LENGTH / CELL_NUM));
            }
            else {
                // from right to left
                // ---
                // |\|
                // ---
                glTexCoord2d(1, 1); glVertex3f((j + 1) * (WIDTH / CELL_NUM), snowed_height_map[i][j + 1], i * (LENGTH / CELL_NUM));
                glTexCoord2d(1, 0); glVertex3f((j + 1) * (WIDTH / CELL_NUM), snowed_height_map[i + 1][j + 1], (i + 1) * (LENGTH / CELL_NUM));
                glTexCoord2d(0, 1); glVertex3f(j * (WIDTH / CELL_NUM), snowed_height_map[i][j], i * (LENGTH / CELL_NUM));
                glTexCoord2d(0, 0); glVertex3f(j * (WIDTH / CELL_NUM), snowed_height_map[i + 1][j], (i + 1) * (LENGTH / CELL_NUM));
            }
            glEnd();

            if (cell_map[i][j].hit_num > 0 && cell_map[i][j].hit_num < HIT_LIMIT) {
                glBindTexture(GL_TEXTURE_2D, texture_id[2]);
                glPushAttrib(GL_CURRENT_BIT);
                glEnable(GL_BLEND);
                for (int ii = 0; ii < cell_map[i][j].hit_num; ++ii) {
                    Particle &p = cell_map[i][j].particle[ii];
                    Vector3 normal, e1(0, 0, 1), e2;
                    float X = p.x / (WIDTH / CELL_NUM) - j;
                    float Z = p.z / (LENGTH / CELL_NUM) - i;
                    if (i % 2 == 0) {
                        if (X + Z > 1) {
                            //   -
                            //  /|
                            // ---
                            normal.x = (LENGTH / CELL_NUM) * (snowed_height_map[i + 1][j] - snowed_height_map[i + 1][j + 1]);
                            normal.y = (WIDTH / CELL_NUM) * (LENGTH / CELL_NUM);
                            normal.z = (WIDTH / CELL_NUM) * (snowed_height_map[i][j + 1] - snowed_height_map[i + 1][j + 1]);
                            float XX = (p.x - p.size) / (WIDTH / CELL_NUM) - j;
                            float ZZ = (p.z - p.size) / (LENGTH / CELL_NUM) - i;
                            if (XX + ZZ <= 1) {
                                p.x += p.size;
                                p.z += p.size;
                            }
                        }
                        else {
                            // ---
                            // |/
                            // -
                            normal.x = (LENGTH / CELL_NUM) * (snowed_height_map[i][j] - snowed_height_map[i][j + 1]);
                            normal.y = (WIDTH / CELL_NUM) * (LENGTH / CELL_NUM);
                            normal.z = (WIDTH / CELL_NUM) * (snowed_height_map[i][j] - snowed_height_map[i + 1][j]);
                            float XX = (p.x + p.size) / (WIDTH / CELL_NUM) - j;
                            float ZZ = (p.z + p.size) / (LENGTH / CELL_NUM) - i;
                            if (XX + ZZ > 1) {
                                p.x -= p.size;
                                p.z -= p.size;
                            }
                        }
                    }
                    else {
                        if (Z > X) {
                            // -
                            // |\
                            // ---
                            normal.x = (LENGTH / CELL_NUM) * (snowed_height_map[i + 1][j] - snowed_height_map[i + 1][j + 1]);
                            normal.y = (WIDTH / CELL_NUM) * (LENGTH / CELL_NUM);
                            normal.z = (WIDTH / CELL_NUM) * (snowed_height_map[i][j] - snowed_height_map[i + 1][j]);
                            float XX = (p.x + p.size) / (WIDTH / CELL_NUM) - j;
                            float ZZ = (p.z - p.size) / (LENGTH / CELL_NUM) - i;
                            if (ZZ <= XX) {
                                p.x -= p.size;
                                p.z += p.size;
                            }
                        }
                        else {
                            // ---
                            //  \|
                            //   -
                            normal.x = (LENGTH / CELL_NUM) * (snowed_height_map[i][j] - snowed_height_map[i][j + 1]);
                            normal.y = (WIDTH / CELL_NUM) * (LENGTH / CELL_NUM);
                            normal.z = (WIDTH / CELL_NUM) * (snowed_height_map[i][j + 1] - snowed_height_map[i + 1][j + 1]);
                            float XX = (p.x - p.size) / (WIDTH / CELL_NUM) - j;
                            float ZZ = (p.z + p.size) / (LENGTH / CELL_NUM) - i;
                            if (ZZ > XX) {
                                p.x += p.size;
                                p.z -= p.size;
                            }
                        }
                    }
                    normal.normalize();
                    e2 = cross_product(normal, e1);
                    e2.normalize();
                    e1 = cross_product(e2, normal);
                    e1.normalize();

                    p.y = snowed_height(p.x + WIDTH / 2, p.z + LENGTH / 2) + 1;
                    glColor4ub(p.r, p.g, p.b, 255);
                    glNormal3f(normal.x, normal.y, normal.z);
                    glBegin(GL_QUADS);
                        glTexCoord2f(0, 0);
                        glVertex3f(p.x + WIDTH / 2 - p.size * (e1.x + e2.x), p.y - p.size * (e1.y + e2.y), p.z + LENGTH / 2 - p.size * (e1.z + e2.z));
                        glTexCoord2f(1, 0);
                        glVertex3f(p.x + WIDTH / 2 + p.size * (e1.x - e2.x), p.y + p.size * (e1.y - e2.y), p.z + LENGTH / 2 + p.size * (e1.z - e2.z));
                        glTexCoord2f(1, 1);
                        glVertex3f(p.x + WIDTH / 2 + p.size * (e1.x + e2.x), p.y + p.size * (e1.y + e2.y), p.z + LENGTH / 2 + p.size * (e1.z + e2.z));
                        glTexCoord2f(0, 1);
                        glVertex3f(p.x + WIDTH / 2 - p.size * (e1.x - e2.x), p.y - p.size * (e1.y - e2.y), p.z + LENGTH / 2 - p.size * (e1.z - e2.z));
                    glEnd();
                }
                glDisable(GL_BLEND);
                glPopAttrib();
            }
        }
}

float Terrain::org_height(float x, float z) {
    //用双线性插值求高度
    int i = int(z / (LENGTH / CELL_NUM));
    int j = int(x / (WIDTH / CELL_NUM));

    float X = x / (WIDTH / CELL_NUM) - j;
    float Z = z / (LENGTH / CELL_NUM) - i;
    return org_height_map[i][j] * (1 - X) * (1 - Z) +
            org_height_map[i + 1][j] * (1 - X) * Z +
            org_height_map[i][j + 1] * X * (1 - Z) +
            org_height_map[i + 1][j + 1] * X * Z;
}

float Terrain::snowed_height(float x, float z) {
    //用双线性插值求高度
    int i = int(z / (LENGTH / CELL_NUM));
    int j = int(x / (WIDTH / CELL_NUM));

    float X = x / (WIDTH / CELL_NUM) - j;
    float Z = z / (LENGTH / CELL_NUM) - i;
    return snowed_height_map[i][j] * (1 - X) * (1 - Z) +
            snowed_height_map[i + 1][j] * (1 - X) * Z +
            snowed_height_map[i][j + 1] * X * (1 - Z) +
            snowed_height_map[i + 1][j + 1] * X * Z;
}

float Terrain::getHighlightX() {
    return (highlightX + 0.5) * (WIDTH / CELL_NUM);
}

float Terrain::getHighlightZ() {
    return (highlightZ + 0.5) * (LENGTH / CELL_NUM);
}

bool Terrain::plantable() {
    return highlightX >= 0 && highlightZ >= 0;
}

bool Terrain::snow_hit(Particle p) {
    int i = int((p.z + LENGTH / 2) / (LENGTH / CELL_NUM));
    int j = int((p.x + WIDTH / 2) / (WIDTH / CELL_NUM));

    if (cell_map[i][j].hit_num < HIT_LIMIT) {
        cell_map[i][j].particle[cell_map[i][j].hit_num++] = p;
        return true;
    }
    else {
        cell_map[i][j].snow_height += SNOW_HEIGHT;
        snowed_height_map[i][j] += SNOW_HEIGHT;
        snowed_height_map[i][j + 1] += SNOW_HEIGHT;
        snowed_height_map[i + 1][j] += SNOW_HEIGHT;
        snowed_height_map[i + 1][j + 1] += SNOW_HEIGHT;
        return true;
    }
    return false;
}

void Terrain::setSnowing(bool value)
{
    snowing = value;
}
