#pragma once


#include "Mesh.h"
#include "RenderItem.h"

using namespace p3d;


class Arrows : public RenderItem, private QOpenGLFunctions {
private:
    std::vector<ColoredPoint> points;
    GLuint u_mvp;
    GLuint a_color;
    GLuint a_position;

    QOpenGLShaderProgram* program;
    void generate_points();
public:

    Arrows();

    void render(const QMatrix4x4 &mvp) override;

    void init_gl() override;
};


