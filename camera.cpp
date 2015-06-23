#include "camera.h"
#include "terrain.h"
#include "gardenwidget.h"

#include <QtCore/qmath.h>
#include <GL/glu.h>

const float MOVE_STEP = 16 * 2;
const float LOOK_RANGE = 45 * M_PI / 180;

const float BORDER = 16;

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    eyeX = eyeZ = 0;
    theta = phi = 0;
    select_terrain = false;
}

void Camera::init(Terrain *t) {
    terrain = t;
    eyeY = terrain->snowed_height(eyeX + WIDTH / 2, eyeZ + LENGTH / 2) + HEIGHT / 24;
}

void Camera::toggle_select_terrain() {
    if (!select_terrain)
        terrain->highlight(eyeX + WIDTH/2, eyeY, eyeZ + LENGTH/2, theta, phi);
    else
        terrain->disable_highlight();
    select_terrain = !select_terrain;
}

void Camera::check_border() {
    if (eyeX < -WIDTH/2 + BORDER)
        eyeX = -WIDTH/2 + BORDER;
    if (eyeZ < -LENGTH/2 + BORDER)
        eyeZ = -LENGTH/2 + BORDER;
    if (eyeX > WIDTH/2 - BORDER)
        eyeX = WIDTH/2 - BORDER;
    if (eyeZ > LENGTH/2 - BORDER)
        eyeZ = LENGTH/2 - BORDER;
    if (eyeY > HEIGHT - BORDER)
        eyeY = HEIGHT - BORDER;
    if (eyeY < BORDER)
        eyeY = BORDER;
}
float Camera::getPhi() const
{
    return phi;
}

void Camera::setPhi(float value)
{
    phi = value;
}

float Camera::getTheta() const
{
    return theta;
}

void Camera::setTheta(float value)
{
    theta = value;
}


void Camera::move(Direction dir) {
    switch (dir) {
    case LEFT:
        eyeX -= MOVE_STEP * qCos(theta);
        eyeZ += MOVE_STEP * qSin(theta);
        break;
    case RIGHT:
        eyeX += MOVE_STEP * qCos(theta);
        eyeZ -= MOVE_STEP * qSin(theta);
        break;
    case FORWARD:
        eyeX -= MOVE_STEP * qSin(theta);
        eyeZ -= MOVE_STEP * qCos(theta);
        break;
    case BACKWARD:
        eyeX += MOVE_STEP * qSin(theta);
        eyeZ += MOVE_STEP * qCos(theta);
        break;
    //上下移动不受地形高度限制
    case UP:
        eyeY += MOVE_STEP;
        check_border();
        if (select_terrain)
            terrain->highlight(eyeX + WIDTH/2, eyeY, eyeZ + LENGTH/2, theta, phi);
        return;
    case DOWN:
        eyeY -= MOVE_STEP;
        check_border();
        if (select_terrain)
            terrain->highlight(eyeX + WIDTH/2, eyeY, eyeZ + LENGTH/2, theta, phi);
        return;
    default:
        break;
    }
    check_border();
    eyeY = terrain->snowed_height(eyeX + WIDTH / 2, eyeZ + LENGTH / 2) + HEIGHT / 24;
    if (select_terrain)
        terrain->highlight(eyeX + WIDTH/2, eyeY, eyeZ + LENGTH/2, theta, phi);
}

void Camera::look(Direction dir, float rate) {
    switch (dir) {
    case LEFT:
        theta += rate * LOOK_RANGE;
        break;
    case RIGHT:
        theta -= rate * LOOK_RANGE;
        break;
    case UP:
        phi -= rate * LOOK_RANGE;
        if (phi > 89.999 * M_PI / 180)
            phi = 89.999 * M_PI / 180;
        if (phi < -89.999 * M_PI / 180)
            phi = -89.999 * M_PI / 180;
        break;
    case DOWN:
        phi += rate * LOOK_RANGE;
        if (phi > 89.999 * M_PI / 180)
            phi = 89.999 * M_PI / 180;
        if (phi < -89.999 * M_PI / 180)
            phi = -89.999 * M_PI / 180;
        break;
    default:
        break;
    }
    if (select_terrain)
        terrain->highlight(eyeX + WIDTH/2, eyeY, eyeZ + LENGTH/2, theta, phi);
}

void Camera::setCamera() {
    //左转theta为正
    //俯视phi为正
    gluLookAt(eyeX, eyeY, eyeZ,
              eyeX - qCos(phi) * qSin(theta), eyeY - qSin(phi), eyeZ - qCos(phi) * qCos(theta),
              0, 1, 0);
}
