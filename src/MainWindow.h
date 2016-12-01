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
        show();
    }

};
