#pragma once

#include <QOpenGLFunctions>
#include "RenderItem.h"
#include "Mesh.h"

using namespace p3d;
class TransformDisplay:public RenderItem, private QOpenGLFunctions {
    Vector3 vec1;
    Vector3 vec2;
    float angle;
    std::vector<ColoredPoint> color_points;
    QOpenGLShaderProgram* program;
    GLuint u_mvp;
    GLuint a_color;
    GLuint a_position;
    float rad;

private:
    void generate_points();

    void set_params(const Vector3& vector);

public:
    TransformDisplay(const Vector3& vec, float angle, float rad=1);

    virtual ~TransformDisplay() {
        delete program;
    }

    void render(const QMatrix4x4 &mvp) override;

    void init_gl() override;
};

