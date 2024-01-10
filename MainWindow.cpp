//
// Created by Daisuke Sakurai on 2024/01/05.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "NoteViewsArea.h"
#include "ui_MainWindow.h"
#include "Settings.h"

#include <QShortcut>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    auto error = Settings::ensure_user_defaults();
//    qDebug() << "error: " << error;
//    if (error & Settings::Error::quit_requested_by_user) {
//        emit quit_from_constructor();
//    }

    // Create a new note
    QShortcut *shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, &QShortcut::activated,
            [this]() {
                QString dir_path = ui->notesListView->model()->rootPath();
                QDir dir (dir_path);
                QString baseName = dir.path() + QDir::separator() + "Untitled";
                QString extension = ".md";
                int counter = 1;

                QString file_path = baseName + QString::number(counter) + extension;
                QFile file(file_path);

                while (file.exists()) {
                    counter++;
                    file_path = baseName + QString::number(counter) + extension;
                    file.setFileName(file_path);
                }

                if (!file.open(QIODevice::WriteOnly)) {
                    /* Display warning */ QMessageBox::critical(this, "File Creation Fail", "Creating a new file failed. Check the folder permission.");
                    return;
                }
                file.close();
                
                // Select file in list view now that the file is created.
                auto index = ui->notesListView->model()->index(file_path);               
                if (!index.isValid()) return;
                
                ui->notesListView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
                ui->notesListView->scrollTo(index);
            }
    );

    connect(
            ui->notesListView, &NotesListView::singleFileSelected,
            ui->noteViewsArea, &NoteViewsArea::open_file
    );
}

MainWindow::~MainWindow() {
    delete ui;
}

NotesListView *MainWindow::notesListView() {
    return ui->notesListView;
}
