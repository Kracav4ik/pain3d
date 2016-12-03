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
    GLuint pos_attr;
    GLuint col_attr;
    GLuint m_attr;
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

        GLubyte indices[] = {
                1,0,2,  1,3,2,
                0,1,5,  0,4,5,
                0,2,6,  0,4,6,
                1,2,6,  0,7,6,
                4,5,7,  4,6,7,
                1,5,7,  1,3,7,
        };

        glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &points[0].pos);
        glVertexAttribPointer(col_attr, 4, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &points[0].col);

        QMatrix4x4 m4;

        m4.scale(.5f);
        static float a = 0;
        m4.rotate(a, 2, 5);
        a += 1;

        program->setUniformValue(m_attr, m4);

        glEnableVertexAttribArray(pos_attr);
        glEnableVertexAttribArray(col_attr);

        glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_BYTE, indices);

        glDisableVertexAttribArray(pos_attr);
        glDisableVertexAttribArray(col_attr);


        program->release();
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 position;
uniform mat4 m;
attribute vec4 color;
varying vec4 v_color;

void main(){
    gl_Position = m*vec4(position, 1.0);
    v_color = color;
}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
varying vec4 v_color;

void main(){
    gl_FragColor = v_color;
}
)");
        program->link();
        pos_attr = program->attributeLocation("position");
        col_attr = program->attributeLocation("color");
        m_attr = program->uniformLocation("m");

    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
        QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
        timer.start(10);
    }

};

