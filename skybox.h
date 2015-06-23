#ifndef SKYBOX_H
#define SKYBOX_H

#include <QObject>
#include <QImage>
#include <QGLWidget>

class SkyBox : public QObject
{
    Q_OBJECT
public:
    explicit SkyBox(QObject *parent = 0);
    ~SkyBox();
    void render();
    void init();
signals:

public slots:

private:
    QImage texture[6];
    GLuint texture_id[6];
};

#endif // SKYBOX_H
