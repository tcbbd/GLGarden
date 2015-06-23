#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

//copy from math.h
#define M_PI		3.14159265358979323846

class Terrain;

class Camera : public QObject
{
    Q_OBJECT
public:
    enum Direction {LEFT, RIGHT, FORWARD, BACKWARD, UP, DOWN};

    explicit Camera(QObject *parent = 0);
    void init(Terrain *t);
    void setCamera();
    void move(Direction dir);
    void look(Direction dir, float rate);
    void toggle_select_terrain();
    float getTheta() const;
    void setTheta(float value);

    float getPhi() const;
    void setPhi(float value);

signals:

public slots:

private:
    void check_border();

    float eyeX, eyeY, eyeZ;
    float theta, phi;
    Terrain *terrain;

    bool select_terrain;
};

#endif // CAMERA_H
