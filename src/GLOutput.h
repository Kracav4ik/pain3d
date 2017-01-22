#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QWheelEvent>
#include "RenderItem.h"
#include "Grid.h"
#include "Mesh.h"
#include "TransformDisplay.h"
#include "Arrows.h"


using namespace p3d;


class LightSource{
    Vector3 pos;
    Color col;
public:
    LightSource();
    const Vector3& get_pos();
    const Color& get_col();
};


class Camera{
private:
    float rot_x;
    float rot_y;
    float pos_x;
    float pos_y;
    float pos_z;
    float scale;
public:
    Camera()
            :rot_x(0)
            ,rot_y(0)
            ,pos_x(0)
            ,pos_y(0)
            ,pos_z(-0.7f)
            ,scale(0.3f)
            {}
    void zoom_in();

    void zoom_out();

    QMatrix4x4 get_matrix() const;

    void rotate(float angle_x, float angle_y);

    void move_x(float dist);

    void move_y(float dist);
};


class GLOutput : public QOpenGLWidget, private QOpenGLFunctions {
Q_OBJECT
private:
    QTimer timer;
    Camera cam;
    Arrows arrows;
    std::vector<RenderItem*> render_items;
    QOpenGLShaderProgram* program;
    GLuint pos_attr;
    QMatrix4x4 mvp;
    QMatrix4x4 mvp2;
    GLuint uv_attr;
    GLuint texture;
    float rot_x2;
    float rot_y2;
    int rot_i;
    GLuint tex_attr;
    GLuint u_mvp;
    QPoint pos;
    Mesh mesh;

protected:
    virtual void wheelEvent(QWheelEvent* event) override;

    virtual void mouseMoveEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void refresh_mvp();

    virtual void paintGL() override;

    virtual void initializeGL() override;

public:
    GLOutput(QWidget* parent);

    void set_rotate(int i);

    GLuint loadBMP_custom(const char* imagepath);
};

