#pragma once

#include <QOpenGLWidget>
#include <stdio.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>
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
    GLuint uv_attr;
    GLuint scale_attr;
    GLuint texture;
    GLuint tex_attr;
    Mesh mesh;

protected:
    virtual void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);
//        for (int i = 0; i < render_items.size(); ++i) {
//            render_items[i]->render();
//        }
        program->bind();

        GLfloat scale[] = {200.f / width(), 200.f / height()};

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
        glUniform2f(scale_attr, scale[0], scale[1]);
        glBindTexture(GL_TEXTURE_2D, texture);

        glEnableVertexAttribArray(pos_attr);
        glEnableVertexAttribArray(uv_attr);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

        glDisableVertexAttribArray(pos_attr);
        glDisableVertexAttribArray(uv_attr);


        program->release();
        mesh.render();
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(.3, 0, .3, 1);

        texture = loadBMP_custom("../tex.bmp");
        mesh.init_gl(loadBMP_custom("../uvtemplate.bmp"));

        for (int i = 0; i < render_items.size(); ++i) {
            //render_items[i]->init_gl();
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(.74f);

        program = new QOpenGLShaderProgram(this);
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
// Input vertex data, different for all executions of this shader.
attribute vec2 a_position;
uniform vec2 u_scale;
attribute vec2 a_vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main(){

    gl_Position =  vec4(a_position*u_scale, 0, 1);

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
        scale_attr = (GLuint) program->uniformLocation("u_scale");
        tex_attr = (GLuint) program->uniformLocation("myTextureSampler");
    }

public:
    GLOutput(QWidget* parent) :
            QOpenGLWidget(parent) {
        QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
        Grid* grid = new Grid();
        render_items.push_back(grid);
        mesh.load();
//        timer.start(100);
    }

    void set_rotate(int i) {
        // TODO: pass transform in render
//        ((Grid*) render_items[0])->set_rotate(i);
        mesh.set_rotate(i);
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

