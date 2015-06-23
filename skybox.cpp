#include "skybox.h"
#include "gardenwidget.h"

#include <GL/glu.h>

SkyBox::SkyBox(QObject *parent) :
    QObject(parent)
{
    texture[0].load("data/glacier_north.bmp", "BMP");
    texture[1].load("data/glacier_east.bmp",  "BMP");
    texture[2].load("data/glacier_south.bmp", "BMP");
    texture[3].load("data/glacier_west.bmp",  "BMP");
    texture[4].load("data/glacier_up.bmp", "BMP");
    texture[5].load("data/glacier_down.bmp", "BMP");
}

SkyBox::~SkyBox() {
    glDeleteTextures(6, texture_id);
}

void SkyBox::init() {
    glGenTextures(6, texture_id);
    for (int i = 0; i < 6; ++i) {
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

void SkyBox::render() {
    glPushMatrix();
    glTranslatef(-WIDTH/2, 0, -LENGTH/2);

    //north
    glBindTexture(GL_TEXTURE_2D, texture_id[0]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(    0, HEIGHT, 0);
        glTexCoord2f(1, 1); glVertex3f(WIDTH, HEIGHT, 0);
        glTexCoord2f(1, 0); glVertex3f(WIDTH,      0, 0);
        glTexCoord2f(0, 0); glVertex3f(    0,      0, 0);
    glEnd();

    //east
    glBindTexture(GL_TEXTURE_2D, texture_id[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(WIDTH, HEIGHT,      0);
        glTexCoord2f(1, 1); glVertex3f(WIDTH, HEIGHT, LENGTH);
        glTexCoord2f(1, 0); glVertex3f(WIDTH,      0, LENGTH);
        glTexCoord2f(0, 0); glVertex3f(WIDTH,      0,      0);
    glEnd();

    //south
    glBindTexture(GL_TEXTURE_2D, texture_id[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(WIDTH, HEIGHT, LENGTH);
        glTexCoord2f(1, 1); glVertex3f(    0, HEIGHT, LENGTH);
        glTexCoord2f(1, 0); glVertex3f(    0,      0, LENGTH);
        glTexCoord2f(0, 0); glVertex3f(WIDTH,      0, LENGTH);
    glEnd();

    //west
    glBindTexture(GL_TEXTURE_2D, texture_id[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(0, HEIGHT, LENGTH);
        glTexCoord2f(1, 1); glVertex3f(0, HEIGHT,      0);
        glTexCoord2f(1, 0); glVertex3f(0,      0,      0);
        glTexCoord2f(0, 0); glVertex3f(0,      0, LENGTH);
    glEnd();

    //sky
    glBindTexture(GL_TEXTURE_2D, texture_id[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(    0, HEIGHT, LENGTH);
        glTexCoord2f(1, 0); glVertex3f(WIDTH, HEIGHT, LENGTH);
        glTexCoord2f(1, 1); glVertex3f(WIDTH, HEIGHT,      0);
        glTexCoord2f(0, 1); glVertex3f(    0, HEIGHT,      0);
    glEnd();

    //ground
    glBindTexture(GL_TEXTURE_2D, texture_id[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f(WIDTH, 0, LENGTH);
        glTexCoord2f(1, 1); glVertex3f(    0, 0, LENGTH);
        glTexCoord2f(1, 0); glVertex3f(    0, 0,      0);
        glTexCoord2f(0, 0); glVertex3f(WIDTH, 0,      0);
    glEnd();

    glPopMatrix();
}
