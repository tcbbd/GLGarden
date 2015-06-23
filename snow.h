#ifndef SNOW_H
#define SNOW_H

#include <QObject>
#include <QGLWidget>

const int SNOWNUM_MAX = 25;

class Terrain;
class Camera;
class NaiveTree;

struct Particle {
    Particle() { init(); }
    void init();

    float x, y, z;
    unsigned int r, g, b;
    float vx, vy, vz;
    float ax, ay, az;
    float size;
    bool isdead;
};

class Snow : public QObject
{
    Q_OBJECT
public:
    explicit Snow(QObject *parent = 0);
    ~Snow();
    void init(Terrain *terrain, Camera *camera, QVector<NaiveTree*> *naiveforest);
    void render();

    float getWind_x() const;
    void setWind_x(float value);
    float getWind_y() const;
    void setWind_y(float value);
    float getWind_z() const;
    void setWind_z(float value);

    int getSnow_perframe() const;
    void setSnow_perframe(int value);

signals:

public slots:

private:
    void update_snow(Particle &p, int &count);

    QVector<Particle> particles;
    QImage texture;
    GLuint texture_id;

    int snow_perframe;
    float wind_x, wind_y, wind_z;

    Terrain *terrain;
    Camera *camera;
    QVector<NaiveTree*> *naiveforest;
};

#endif // SNOW_H
