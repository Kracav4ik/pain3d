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

protected:
    virtual void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < render_items.size(); ++i) {
            render_items[i]->render();
        }
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
        initializeOpenGLFunctions();

        for (int i = 0; i < render_items.size(); ++i) {
            render_items[i]->init_gl();
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(.74f);

    }

public:
    GLOutput(QWidget* parent): QOpenGLWidget(parent) {
        QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
        Grid* grid = new Grid();
        render_items.push_back(grid);
//        timer.start(100);
    }

    void set_rotate(int i){
        // TODO: pass transform in render
        ((Grid*)render_items[0])->set_rotate(i);
    }

};

