#ifndef TERRAIN_H
#define TERRAIN_H

#include <array>
#include <QObject>
#include <QGLWidget>

#include "snow.h"

const int CELL_NUM = 32;

static const int HIT_LIMIT = 5;
static const float SNOW_HEIGHT = 0.3;

struct Vector3 {
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float a, float b, float c) : x(a), y(b), z(c) {}
    void normalize();
    float x, y, z;
};

struct Cell {
    Cell();
    int hit_num;
    float snow_height;
    std::array<Particle, HIT_LIMIT> particle;
};

class Terrain : public QObject
{
    Q_OBJECT
public:
    explicit Terrain(QObject *parent = 0);
    ~Terrain();
    void init();
    void render();
    float org_height(float x, float z);
    float snowed_height(float x, float z);
    void highlight(float eyeX, float eyeY, float eyeZ, float theta, float phi);
    float getHighlightX();
    float getHighlightZ();
    void disable_highlight();
    bool plantable();

    bool snow_hit(Particle p);
    void setSnowing(bool value);

signals:

public slots:

private:
    void draw_snow();

    QImage texture[3];
    GLuint texture_id[3];
    float org_height_map[CELL_NUM + 1][CELL_NUM + 1];
    float snowed_height_map[CELL_NUM + 1][CELL_NUM + 1];
    Cell cell_map[CELL_NUM][CELL_NUM];
    int highlightZ, highlightX;

    bool snowing;
    int frame_count;
};

#endif // TERRAIN_H
