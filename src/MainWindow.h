#pragma once

#include <QMainWindow>
#include <QFontDialog>
#include <QFileDialog>

#include "ui_pain3d.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT
public:
    MainWindow() {
        setupUi(this);
        on_verticalSlider_valueChanged(verticalSlider->value());

        show();
    }

public slots:
    void on_verticalSlider_valueChanged(int i){
        glWidget->set_rotate(i);
        glWidget->update();
    }


};
