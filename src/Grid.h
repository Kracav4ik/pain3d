#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "RenderItem.h"
#include "Mesh.h"

using namespace p3d;

struct ColoredPoint {
    ColoredPoint(const Vertex3& v, const Color& c) :
            vertex(v),
            color(c) {}

    Vertex3 vertex;
    Color color;
};

class Grid : public RenderItem, private QOpenGLFunctions {
private:
    QOpenGLShaderProgram* program;
    GLuint u_mvp;
    GLuint a_color;
    GLuint a_position;
    std::vector<ColoredPoint> color_points;
public:
    Grid()
        : program(nullptr)
        , u_mvp((GLuint) -1)
        , a_color((GLuint) -1)
        , a_position((GLuint) -1)
    {
        generate_points(20);
    }

    ~Grid() {
        delete program;
    }

    void generate_points(unsigned int squares_count) {
        for (int i = 0; i <= squares_count; ++i) {
            color_points.push_back(ColoredPoint(Vertex3(10 - i, -10, 0), Color(1, 1, 1)));
            color_points.push_back(ColoredPoint(Vertex3(10 - i, 10, 0), Color(1, 0, 1)));
            color_points.push_back(ColoredPoint(Vertex3(-10, 10 - i, 0), Color(1, 1, 0)));
            color_points.push_back(ColoredPoint(Vertex3(10, 10 - i, 0), Color(1, 0, 0)));
        }
        color_points.push_back(ColoredPoint(Vertex3(0, 0, 15), Color(0, 0, 1)));
        color_points.push_back(ColoredPoint(Vertex3(0, 0, -5), Color(0, 1, 0)));
    }

    virtual void render(const QMatrix4x4& mvp) override {
        program->bind();

        glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &color_points.data()[0].vertex);
        glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &color_points.data()[0].color);

        program->setUniformValue(u_mvp, mvp);

        glEnableVertexAttribArray(a_position);
        glEnableVertexAttribArray(a_color);

        glDrawArrays(GL_LINES, 0, color_points.size());

        glDisableVertexAttribArray(a_position);
        glDisableVertexAttribArray(a_color);

        program->release();

    }

    virtual void init_gl() override {
        initializeOpenGLFunctions();

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1.2f);
        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_position;
attribute vec4 a_color;
uniform mat4 u_mvp;

out vec4 COL;

void main(){
    COL = a_color;
    gl_Position = u_mvp*vec4(a_position, 1.0);
}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
in vec4 COL;
void main(){
    gl_FragColor = COL;
}
)");
        program->link();
        a_position = (GLuint) program->attributeLocation("a_position");
        a_color = (GLuint) program->attributeLocation("a_color");
        u_mvp = (GLuint) program->uniformLocation("u_mvp");
    }


};
