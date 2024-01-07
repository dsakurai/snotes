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

void open_main_window() {

    QSettings settings;
    QVariant notes_folder = settings.value(Settings::Keys::UserDefaultsGroup::notes_folder);
    
    if (not notes_folder.isValid()) show_warning_and_request_quit("Failed to read the notes folder from the settings. Quitting app.");
    
    if (not QDir(notes_folder.toString()).exists()) show_warning_and_request_quit("The notes folder does not exist. Quitting app.");
    
    auto *mainWindow = new MainWindow();
    auto *model      = new NotesFolderModel{};

    model->setRootPath(notes_folder.toString());
    mainWindow->notesListView()->setProjectFolderModel(model);

    mainWindow->show();
}

/**
 * Because the input `path` must not be destroyed before the dialog is shown,
 * this function should be called only from `main()` where `path` lives forever. 
 * @param path 
 */
void ensure_user_folder_and_request_main_window(QVariant& path) {

    QSettings qsettings;

    path = qsettings.value(Settings::Keys::UserDefaultsGroup::notes_folder);

    if ( not path.isValid() ||
         not QDir(path.toString()).exists()) {

        ChooseFolderDialog *dialog = Settings::get_user_default_folder();

        // The default path
        path = vendor_defaults::notes_folder_qdir().path();

        // User can override the default
        QObject::connect(
                dialog, &ChooseFolderDialog::folder_set,
                [&path](const QString preferred_path) {
                    path = preferred_path;
                }
        );

        // Dialog is closed
        QObject::connect(
                dialog, &ChooseFolderDialog::finished,
                [&path](int result) {
                    // OK button pushed
                    if (result == QDialog::Accepted) {

                        if ( not path.isValid() ) { // i.e. is not null
                            show_warning_and_request_quit("Failed to get the path to the folder. Quitting app.");
                            return;
                        }

                        QString path_string = path.toString();

                        if ( not QDir(path_string).exists()) {
                            const int success = QDir{}.mkpath(path_string);
                            if (not success) {
                                show_warning_and_request_quit("Failed to create folder. Quitting app.");
                                return;
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

                                bool success = markdown.copy(QDir(path_string).filePath(name));

                                if (not success) {
                                    show_warning_and_request_quit("Failed to create notes in the folder. Quitting the app.");
                                    return;
                                }
                            } else {
                                show_warning_and_request_quit("Failed to create notes in the folder. Quitting the app.");
                                return;
                            }
                        }

                        // Save path
                        QSettings settings;
                        settings.setValue(Settings::Keys::UserDefaultsGroup::notes_folder, path_string);
                        
                        open_main_window();
                    } else { // The user refused to set a project folder.
                        QSettings settings;
                        settings.remove(Settings::Keys::UserDefaultsGroup::notes_folder);
                        QCoreApplication::quit();
                    }
                }
        );

        dialog->open();

    } else {
        open_main_window();
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    about_this_app::setOrganizationName("SNoteTeam");
    about_this_app::setApplicationName("SNote");
    
    QVariant path;

    // Execute when event loop starts
    QTimer::singleShot(0,
                       // In case the folder does not exist, the main window opens only after this function returns.
                       // This is due to Qt's event loop mechanism 
                       [&path]() {
                           ensure_user_folder_and_request_main_window(path);
                       }
    );
    
    return QApplication::exec();
}
