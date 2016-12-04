#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>

namespace p3d {
    struct Vector3 {
        union {
            struct {
                float x, y, z;
            };
            float v[3];
        };
    };

    struct Color {
        union {
            struct {
                float r, g, b, a;
            };
            float c[4];
        };
    };

    struct MeshPoint {
        Vector3 pos;
        Color   col;
    };
}

using namespace p3d;

class GLOutput : public QOpenGLWidget, private QOpenGLFunctions {
Q_OBJECT
private:
    QOpenGLShaderProgram* program;
    GLuint a_position;
    GLuint a_color;
    GLuint u_color;
    GLuint u_transform;
    int angel;
    QTimer timer;

protected:
    virtual void paintGL() override {
        glClearColor(0.5, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        program->bind();

        GLfloat scale[] = {.5f, .5f, .5f};

        MeshPoint points[] = {
                -1, -1,  1,   0, 0, 1, 1,
                -1,  1,  1,   0, 1, 1, 1,
                 1, -1,  1,   1, 0, 1, 1,
                 1,  1,  1,   1, 1, 1, 1,
                -1, -1, -1,   0, 0, 0, 1,
                -1,  1, -1,   0, 1, 0, 1,
                 1, -1, -1,   1, 0, 0, 1,
                 1,  1, -1,   1, 1, 0, 1,
        };

        GLubyte triangle_indices[] = {
                1,0,2,  1,3,2,
                0,1,5,  0,4,5,
                0,2,6,  0,4,6,
                2,3,7,  2,7,6,
                4,5,7,  4,6,7,
                1,5,7,  1,3,7,
        };

        GLubyte outline_indices[] = {
                0, 1,  1, 3,
                3, 2,  2, 0,
                4, 5,  5, 7,
                7, 6,  6, 4,
                0, 4,  1, 5,
                3, 7,  2, 6,
        };

        glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &points[0].pos);
        glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &points[0].col);

        QMatrix4x4 m4;

        m4.scale(.5f);
        m4.rotate(angel, 2, 5);

        program->setUniformValue(u_transform, m4);

        glEnableVertexAttribArray(a_position);
        glEnableVertexAttribArray(a_color);

        program->setUniformValue(u_color, 1,1,1,1);
        glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_BYTE, triangle_indices);
        program->setUniformValue(u_color, 0,0,0,1);
        glDrawElements(GL_LINES, 12*2, GL_UNSIGNED_BYTE, outline_indices);

        glDisableVertexAttribArray(a_position);
        glDisableVertexAttribArray(a_color);


        program->release();
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(.74f);

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_position;
uniform mat4 u_transform;
uniform vec4 u_color;
attribute vec4 a_color;
varying vec4 v_color;

void main(){
    gl_Position = u_transform*vec4(a_position, 1.0);
    v_color = a_color*u_color;
}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
varying vec4 v_color;

void main(){
    gl_FragColor = v_color;
}
)");
        program->link();
        a_position = (GLuint) program->attributeLocation("a_position");
        a_color = (GLuint) program->attributeLocation("a_color");
        u_color = (GLuint) program->uniformLocation("u_color");
        u_transform = (GLuint) program->uniformLocation("u_transform");
    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
        QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
//        timer.start(100);
    }

    void set_rotate(int i){
        angel = i;
    }

};

