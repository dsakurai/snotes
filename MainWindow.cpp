//
// Created by Daisuke Sakurai on 2024/01/05.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "NoteViewsArea.h"
#include "ui_MainWindow.h"
#include "Settings.h"

#include <QShortcut>

void show_warning_and_request_quit(QString message) {
    auto* dialog = new QDialog();

    // app modal
    dialog->setModal(true);

    auto* layout = new QHBoxLayout(dialog);

    layout->addWidget(
            new QLabel(message, dialog)
    );
    auto* ok_button = new QPushButton("OK", dialog);
    layout->addWidget(
            ok_button
    );

    QObject::connect(
            dialog, &QDialog::finished,
            [](int result) {
                QCoreApplication::quit();
            }
    );

    QObject::connect(
            ok_button, &QPushButton::clicked,
            dialog, &QDialog::accept
    );

    dialog->open();
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {

    QSettings settings;
    QVariant notes_folder = settings.value(Settings::Keys::UserDefaultsGroup::notes_folder);

    if (not notes_folder.isValid()) {
        show_warning_and_request_quit("Failed to read the notes folder from the settings. Quitting app.");
        return;
    }
    if (not QDir(notes_folder.toString()).exists()) {
        show_warning_and_request_quit("The notes folder does not exist. Quitting app.");
        return;
    }
    
    ui->setupUi(this);
    
    auto *model      = new NotesFolderModel{};
    model->setRootPath(notes_folder.toString());
    ui->notesListView->setProjectFolderModel(model);
    
//    QObject::connect(
//            mainWindow->ui->searchLineEdit, &QLineEdit::textEdited,
//    );
//    mainWindow->ui->searchLineEdit->

//    model->setFilterWildcard("*3*");
//    model->setFilterFixedString("3");

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
