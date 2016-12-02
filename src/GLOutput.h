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
        glClear(GL_COLOR_BUFFER_BIT);
        program->bind();

        GLfloat scale[] = {100.f/width(), 100.f/height()};

        GLfloat vertices[] = {
                -1,-1,
                -1,1,
                1,-1,
                1,1
        };

        GLfloat colors[] = {
                1,0,0,1,
                0,1,0,1,
                0,0,1,1,
                1,1,0,1
        };

        glVertexAttribPointer(pos_attr,2,GL_FLOAT,GL_FALSE,0,vertices);
        glVertexAttribPointer(col_attr,4,GL_FLOAT,GL_FALSE,0,colors);
        glUniform2f(scale_attr,scale[0], scale[1]);

        glEnableVertexAttribArray(pos_attr);
        glEnableVertexAttribArray(col_attr);

        glDrawArrays(GL_TRIANGLE_STRIP,0,4);

        glDisableVertexAttribArray(col_attr);
        glDisableVertexAttribArray(pos_attr);


        program->release();
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec2 position;
uniform vec2 scale;
attribute vec4 color;
varying vec4 v_color;

void main(){
    gl_Position = vec4(position*scale,0.0,1.0);
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
        scale_attr = program->uniformLocation("scale");

    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
    }

};

