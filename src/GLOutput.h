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


using namespace p3d;

class GLOutput : public QOpenGLWidget, private QOpenGLFunctions {
Q_OBJECT
private:
    QTimer timer;
    std::vector<RenderItem*> render_items;
    QOpenGLShaderProgram* program;
    GLuint pos_attr;
    QMatrix4x4 mvp;
    GLuint uv_attr;
    GLuint texture;
    float scale;
    float rot_x;
    float rot_y;
    int rot_i;
    GLuint tex_attr;
    GLuint u_mvp;
    QPoint pos;
    Mesh mesh;

protected:
    virtual void wheelEvent(QWheelEvent* event) override {
        if (event->angleDelta().y() > 0) {
            scale *= 1.2;
        } else {
            scale /= 1.2;
        }
        refresh_mvp();
    }

    virtual void mouseMoveEvent(QMouseEvent* event) override {
        QPoint point = event->pos();
        if (pos.isNull()) {
            pos = point;
            return;
        }
        if (event->buttons() & Qt::LeftButton) {
            QPoint delta = pos - point;
            rot_x += delta.x();
            rot_y += delta.y();
            refresh_mvp();
        }
        pos = point;
    }

    void refresh_mvp() {
        mvp.setToIdentity();
        mvp.scale(scale);
        mvp.rotate(rot_x, 0, 1, 0);
        mvp.rotate(rot_y, 1, 0, 0);
        mvp.rotate(rot_i, 1, 1, 1);
        update();
    }

    virtual void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < render_items.size(); ++i) {
            render_items[i]->render(mvp);
        }
        program->bind();

        GLfloat vertices[] = {
                -1, -1, 0, 0,
                -1, 1, 0, 1,
                1, -1, 1, 0,
                1, 1, 1, 1,
        };

        GLubyte indices[] = {
                0, 1, 2, 1, 2, 3
        };

        glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
        glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);

        program->setUniformValue(u_mvp, mvp);

        glBindTexture(GL_TEXTURE_2D, texture);

        glEnableVertexAttribArray(pos_attr);
        glEnableVertexAttribArray(uv_attr);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

        glDisableVertexAttribArray(pos_attr);
        glDisableVertexAttribArray(uv_attr);


        program->release();
        mesh.render(mvp);
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(.3, 0, .3, 1);

        texture = loadBMP_custom("../tex.bmp");
        mesh.set_tex(loadBMP_custom("../aaa.bmp"));
        mesh.init_gl();

        for (int i = 0; i < render_items.size(); ++i) {
            render_items[i]->init_gl();
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(.74f);

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
// Input vertex data, different for all executions of this shader.
attribute vec2 a_position;
attribute vec2 a_vertexUV;
uniform mat4 u_mvp;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main(){
    gl_Position = u_mvp * vec4(a_position, 0, 1);
    // UV of the vertex. No special space for this one.
    UV = a_vertexUV;
}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

    // Output color = color of the texture at the specified UV
    gl_FragColor = texture2D( myTextureSampler, UV );
    //gl_FragColor = vec4(UV, 1, 1);
}
)");
        program->link();
        pos_attr = (GLuint) program->attributeLocation("a_position");
        uv_attr = (GLuint) program->attributeLocation("a_vertexUV");
        u_mvp = (GLuint) program->uniformLocation("u_mvp");
        tex_attr = (GLuint) program->uniformLocation("myTextureSampler");
    }

public:
    GLOutput(QWidget* parent) :
            QOpenGLWidget(parent),
            scale(0.3),
            rot_x(0),
            rot_y(0) {
        QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
        Grid* grid = new Grid();
        render_items.push_back(grid);
        mesh.load();
        setMouseTracking(true);
//        timer.start(100);
    }

    void set_rotate(int i) {
        rot_i = i;
        refresh_mvp();
    }

    GLuint loadBMP_custom(const char* imagepath) {
        unsigned char header[54];
        unsigned int dataPos;
        unsigned int width, height;
        unsigned int imageSize;
        unsigned char* data;

        FILE* file = fopen(imagepath, "rb");
        fread(header, 1, 54, file);

        dataPos = *(unsigned int*) &(header[0x0A]);
        imageSize = *(unsigned int*) &(header[0x22]);
        width = *(unsigned int*) &(header[0x12]);
        height = *(unsigned int*) &(header[0x16]);
        if (imageSize == 0) {
            imageSize = width * height * 3;
        }
        data = new unsigned char[imageSize];

        fread(data, 1, imageSize, file);

        fclose(file);

        GLuint textureID;
        glGenTextures(1, &textureID);

        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        return textureID;
    }
};

