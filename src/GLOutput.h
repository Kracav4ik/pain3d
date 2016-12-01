#pragma once

#include <QOpenGLWidget>
#include <stdio.h>

class GLOutput : public QOpenGLWidget {
Q_OBJECT
private:

protected:
    virtual void paintGL() override {
    }

    virtual void resizeGL(int w, int h) override {
    }

    virtual void initializeGL() override {
    }

public:
    GLOutput(QWidget* parent)
            : QOpenGLWidget(parent)
    {}


};

