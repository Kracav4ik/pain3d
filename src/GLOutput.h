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

protected:
    virtual void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);
        program->bind();

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

        glEnableVertexAttribArray(pos_attr);
        glEnableVertexAttribArray(col_attr);

        glDrawArrays(GL_TRIANGLE_STRIP,0,4);

        glDisableVertexAttribArray(col_attr);
        glDisableVertexAttribArray(pos_attr);


        program->release();
    }

    virtual void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec2 position;
attribute vec4 color;
varying vec4 v_color;

void main(){
    gl_Position = vec4(position,0.0,1.0);
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

    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
    }


};

