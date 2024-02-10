#include <ryml.hpp>

#include "About_this_app.h"
#include "Defaults.h"
#include "MainWindow.h"
#include "NotesListView.h"
#include "Settings.h"

#include <QApplication>
#include <QFile>
#include <QSortFilterProxyModel>
#include <QStandardPaths>
#include <QTimer>

#include <iostream>
#include <fstream>

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

bool is_pinned(const std::string& path) {

    // These are re-allocated...
    // Better, use an object pool.
    std::string line;
    std::string yaml_header;
    
    if (std::ifstream stream {path};
            stream.is_open() ) { // I could read the file in binary mode for performance, maybe?

        std::getline(stream, line);

        if (yaml_header.size() > 1024) std::string{}.swap(yaml_header); // Free RAM

        // yaml header?
        if (line != "---") return false; // no

        bool found = false;
        while (std::getline(stream, line)) {

            if (line == "---") {
                found = true;
                break;
            }

            yaml_header += line + "\n";
        }

        if(!found) return false;

        ryml::Tree tree = ryml::parse_in_place(ryml::substr(
                yaml_header.data(), yaml_header.size()));

        auto pinned = tree["pinned"];
        if (pinned.valid() && pinned.has_val() ) {
            auto val = pinned.val();
            if ((val == "true") || (val == "yes")) {
            }
        }

    } else {
        qDebug() << "Failed to read file.";
    }
    
    return false;
}

void open_main_window() {

    QSettings settings;
    QVariant notes_folder = settings.value(Settings::Keys::UserDefaultsGroup::notes_folder);
    
    if (not notes_folder.isValid()) show_warning_and_request_quit("Failed to read the notes folder from the settings. Quitting app.");
    
    if (not QDir(notes_folder.toString()).exists()) show_warning_and_request_quit("The notes folder does not exist. Quitting app.");
    
    auto *mainWindow = new MainWindow();
    auto *model      = new NotesFolderModel{};

    model->setRootPath(notes_folder.toString());
    mainWindow->notesListView()->setProjectFolderModel(model);
    
//    QObject::connect(model, &QFileSystemModel::directoryLoaded, [model](const QString  &path) {
//    
//        qDebug() << "dir loaded";
//    
//        auto rootIndex = model->index(model->rootPath());
//        
//        for (int row = 0; row < model->rowCount(rootIndex); ++row) {
//            QModelIndex currentIndex = model->index(row, 0, rootIndex);
//            QString path = model->filePath(currentIndex);
//
//        }
//    });

    mainWindow->show();
}

bool initialize_user_folder(const QVariant& path) {

    if ( not path.isValid() ) { // i.e. is not null
        show_warning_and_request_quit("Failed to get the path to the folder. Quitting app.");
        return false;
    }

    QString path_string = path.toString();

    if ( not QDir(path_string).exists()) {
        const int success = QDir{}.mkpath(path_string);
        if (not success) {
            show_warning_and_request_quit("Failed to create folder. Quitting app.");
            return false;
        }
    }

    // Path exists

    // Create notes
    QDirIterator notes (":/vendor_defaults/notes");
    while (notes.hasNext()) {

        auto note_resource_path = notes.next();
        QFile markdown (note_resource_path);

        if (markdown.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString name = QFileInfo(markdown.fileName()).fileName();

            QFile write_to_file (QDir(path_string).filePath(name));

            // If the file already exists on the disk, leave it alone.
            if (write_to_file.exists()) continue;

            if (write_to_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                write_to_file.write(markdown.readAll());
            } else { // failed to open file for writing.
                show_warning_and_request_quit("Failed to create notes in the folder. Quitting the app.");
                return false;
            }
        } else {
            show_warning_and_request_quit("Failed to create notes in the folder. Quitting the app.");
            return false;
        }
    }

    // Save path
    QSettings settings;
    settings.setValue(Settings::Keys::UserDefaultsGroup::notes_folder, path_string);
    
    return true;
}

/**
 * Because the input `path` must not be destroyed before the dialog is shown,
 * this function should be called only from `main()` where `path` lives forever. 
 * @param path 
 */
void ensure_user_folder_and_request_main_window() {

    QSettings qsettings;

    // This will be passed to lambda functions.
    // The shared_ptr is copied in order to avoid deletion before the lambdas are executed.
    std::shared_ptr path = std::make_shared<QVariant >(qsettings.value(Settings::Keys::UserDefaultsGroup::notes_folder));

    if ( not path->isValid() ||
         not QDir(path->toString()).exists()) {

        ChooseFolderDialog *choose_folder_dialog = Settings::get_user_default_folder();

        // The default path
        *path = vendor_defaults::notes_folder_qdir().path();

        // User can override the default
        QObject::connect(
                choose_folder_dialog, &ChooseFolderDialog::folder_set,
                [path=path](const QString preferred_path) {
                    *path = preferred_path;
                }
        );

        // Initialize the user-default notes folder on dialog closing
        QObject::connect(
                choose_folder_dialog, &ChooseFolderDialog::finished,
                [path=path](int result) {
                    if (result == QDialog::Accepted) { // OK button pushed
                        const int success = initialize_user_folder(*path);
                        if (not success) return; // app will be closed as requested in `initialize_user_folder`
                        open_main_window();

                    } else { // The user refused to set a project folder.
                        QSettings{}.remove(Settings::Keys::UserDefaultsGroup::notes_folder);
                        QCoreApplication::quit();
                    }
                }
        );

        choose_folder_dialog->open();

    } else {
        open_main_window();
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    about_this_app::setOrganizationName("SNoteTeam");
    about_this_app::setApplicationName("SNote");
    
    // Execute when event loop starts
    QTimer::singleShot(0,
                       // In case the folder does not exist, the main window opens only after this function returns.
                       // This is due to Qt's event loop mechanism 
                       ensure_user_folder_and_request_main_window
    );
    
    return QApplication::exec();
}
