#include "snow.h"
#include "gardenwidget.h"
#include "naivetree.h"

#include <QImage>
#include <QtCore/qmath.h>
#include <GL/glu.h>

const float G = 0.05;

void Particle::init() {
    r = g = b = 255;

    x = LENGTH * (float(qrand()) / RAND_MAX) - LENGTH / 2;
    y = HEIGHT * (1.0 - 0.1 * float(qrand()) / RAND_MAX);
    z = WIDTH * (float(qrand()) / RAND_MAX) - WIDTH / 2;

    vy = 15 * (float(qrand())/ RAND_MAX);
    vx = vz = 0;

    ax = 0;
    ay = G;
    az = 0;

    size = 30 + 15 * (float(qrand())/ RAND_MAX);
    isdead = false;
}

Snow::Snow(QObject *parent) :
    QObject(parent)
{
    texture.load("data/snowball.bmp", "BMP");

    snow_perframe = 0;
    wind_x = wind_y = wind_z = 0;
}

Snow::~Snow() {
    glDeleteTextures(1, &texture_id);
}

void Snow::init(Terrain *terrain, Camera *camera, QVector<NaiveTree*> *naiveforest) {
    this->terrain = terrain;
    this->camera = camera;
    this->naiveforest = naiveforest;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    texture = QGLWidget::convertToGLFormat(texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(),
                      0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
}

void Snow::render() {
    glPushAttrib(GL_CURRENT_BIT);
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    float theta = camera->getTheta();
    float phi = camera->getPhi();
    float cosphi = qCos(phi);
    float x00 =  qSin(phi) * qSin(theta) - qCos(theta);
    float x01 = -qSin(phi) * qSin(theta) - qCos(theta);
    float x10 =  qSin(phi) * qSin(theta) + qCos(theta);
    float x11 = -qSin(phi) * qSin(theta) + qCos(theta);
    float z00 =  qSin(phi) * qCos(theta) + qSin(theta);
    float z01 = -qSin(phi) * qCos(theta) + qSin(theta);
    float z10 =  qSin(phi) * qCos(theta) - qSin(theta);
    float z11 = -qSin(phi) * qCos(theta) - qSin(theta);
    int count = 0;
    for (Particle &p : particles) {
        if (p.isdead) {
            if (count < snow_perframe) {
                count++;
                p.init();
            }
            continue;
        }
        glColor4ub(p.r, p.g, p.b, 255);
        glNormal3f(qSin(theta), 0, qCos(theta));
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(p.x + p.size * x00, p.y - p.size * cosphi, p.z + p.size * z00);
            glTexCoord2f(1, 0); glVertex3f(p.x + p.size * x10, p.y - p.size * cosphi, p.z + p.size * z10);
            glTexCoord2f(1, 1); glVertex3f(p.x + p.size * x11, p.y + p.size * cosphi, p.z + p.size * z11);
            glTexCoord2f(0, 1); glVertex3f(p.x + p.size * x01, p.y + p.size * cosphi, p.z + p.size * z01);
        glEnd();

        update_snow(p, count);
    }
    glPopMatrix();
    glDisable(GL_BLEND);
    glPopAttrib();

    for (; count < snow_perframe; ++count)
        particles.push_back(Particle());
}

void Snow::update_snow(Particle &p, int &count) {
    p.x += p.vx;
    p.y -= p.vy;
    p.z += p.vz;

    p.vx += p.ax + wind_x;
    p.vy += p.ay + wind_y;
    p.vz += p.az + wind_z;

    if ((p.x <= -WIDTH / 2 || p.x >= WIDTH / 2) ||
            (p.z <= -LENGTH / 2 || p.z >= LENGTH / 2) ||
            p.y <= 0) {
        if (count < snow_perframe) {
            count++;
            p.init();
        }
        else
            p.isdead = true;
    }
    else if (p.y < terrain->snowed_height(p.x + WIDTH / 2, p.z + LENGTH / 2)) {
        if (terrain->snow_hit(p)) {
            if (count < snow_perframe) {
                count++;
                p.init();
            }
            else
                p.isdead = true;
        }
    }
    else {
        for (NaiveTree *t : *naiveforest) {
            if (t->hit(p, particles)) {
                if (count < snow_perframe) {
                    count++;
                    p.init();
                }
                else
                    p.isdead = true;
            }
        }
    }
}
int Snow::getSnow_perframe() const
{
    return snow_perframe;
}

void Snow::setSnow_perframe(int value)
{
    snow_perframe = value;
    if (value == 0)
        terrain->setSnowing(false);
    else
        terrain->setSnowing(true);
}


float Snow::getWind_x() const
{
    return wind_x;
}

void Snow::setWind_x(float value)
{
    wind_x = value;
}
float Snow::getWind_y() const
{
    return wind_y;
}

void Snow::setWind_y(float value)
{
    wind_y = value;
}
float Snow::getWind_z() const
{
    return wind_z;
}

void Snow::setWind_z(float value)
{
    wind_z = value;
}
