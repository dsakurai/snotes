//
// Created by Daisuke Sakurai on 2024/01/05.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Settings.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    auto error = Settings::ensure_user_defaults();
//    qDebug() << "error: " << error;
//    if (error & Settings::Error::quit_requested_by_user) {
//        emit quit_from_constructor();
//    }
}

MainWindow::~MainWindow() {
    delete ui;
}

NotesListView *MainWindow::notesListView() {
    return ui->notesListView;
}
