#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class GLOutput : public QOpenGLWidget, private QOpenGLFunctions {
Q_OBJECT
private:
    QOpenGLShaderProgram* program;
    GLuint pos_attr;
    GLuint col_attr;
    GLuint scale_attr;

protected:
    virtual void paintGL() override {
        glClearColor(0, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        program->bind();

        GLfloat scale[] = {.5f, .5f, .5f};

        GLfloat vertices[] = {
                -1, -1,  1,
                -1,  1,  1,
                 1, -1,  1,
                 1,  1,  1,
                -1, -1, -1,
                -1,  1, -1,
                 1, -1, -1,
                 1,  1, -1
        };

        GLubyte indices[] = {
                1,0,2,  1,3,2,
                0,1,5,  0,4,5,
                0,2,6,  0,4,6,
                1,2,6,  0,7,6,
                4,5,7,  4,6,7,
                4,5,7,  4,6,7,
                1,5,7,  1,3,7,
        };

        GLfloat colors[] = {
                1,0,0,1,
                0,1,0,1,
                0,0,1,1,
                1,1,0,1
        };

        glVertexAttribPointer(pos_attr,3,GL_FLOAT,GL_FALSE,0,vertices);
        glUniform4fv(col_attr, 1, colors);
        glUniform3fv(scale_attr, 1, scale);

        glEnableVertexAttribArray(pos_attr);

        glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_BYTE, indices);

        glDisableVertexAttribArray(pos_attr);


        program->release();
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 position;
uniform vec3 scale;
uniform vec4 color;
varying vec4 v_color;

void main(){
    gl_Position = vec4(position*scale, 1.0);
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
        col_attr = program->uniformLocation("color");
        scale_attr = program->uniformLocation("scale");

    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
    }

};

