#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "RenderItem.h"
#include "Mesh.h"

using namespace p3d;

class Grid : public RenderItem, private QOpenGLFunctions{
private:
    QOpenGLShaderProgram* program;
    GLuint u_transform;
    GLuint u_color;
    GLuint a_position;
    int angle;
    std::vector<Vector3> points;
public:
    Grid()
        : program(nullptr)
        , u_transform((GLuint) -1)
        , u_color((GLuint) -1)
        , a_position((GLuint) -1)
        , angle(0)
    {
        generate_points(20);
    }

    ~Grid() {
        delete program;
    }

    void set_rotate(int i){
        angle = i;
    }

    void generate_points(unsigned int squares_count) {
        points.resize(4*(squares_count + 1)+2);
        int point_idx = 0;
        for (int i = 0; i <= squares_count; ++i) {
            points[point_idx++].set( 10 - i, -10, 0);
            points[point_idx++].set( 10 - i,  10, 0);
            points[point_idx++].set(-10, 10  - i, 0);
            points[point_idx++].set( 10, 10  - i, 0);
        }
        points[point_idx++].set(0,0,15);
        points[point_idx].set(0,0,-5);
    }

    virtual void render() override {
        program->bind();

        glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, 0, points.data());

        QMatrix4x4 m4;

        m4.scale(.065f);
        m4.rotate(-120, 1, 0);
        m4.rotate(angle, 0, 0, 1);

        program->setUniformValue(u_transform, m4);

        glEnableVertexAttribArray(a_position);

        program->setUniformValue(u_color, 1, 0, 0, 1);
        glDrawArrays(GL_LINES, 0, points.size()-2);

        // TODO: move to another class
        glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, 0, points.data() + points.size()-2);
        program->setUniformValue(u_color, 0, 1, 0, 1);
        glDrawArrays(GL_LINES, 0, 2);

        glDisableVertexAttribArray(a_position);

        program->release();

    }

    virtual void init_gl() override {
        initializeOpenGLFunctions();

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1.2f);
        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_position;
uniform mat4 u_transform;

void main(){
    gl_Position = u_transform*vec4(a_position, 1.0);
}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
uniform vec4 u_color;
void main(){
    gl_FragColor = u_color;
}
)");
        program->link();
        a_position = (GLuint) program->attributeLocation("a_position");
        u_transform = (GLuint) program->uniformLocation("u_transform");
        u_color = (GLuint) program->uniformLocation("u_color");
    }


};
