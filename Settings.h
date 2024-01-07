//
// Created by Daisuke Sakurai on 2024/01/07.
//

#ifndef SIMPLEMARKDOWN_SETTINGS_H
#define SIMPLEMARKDOWN_SETTINGS_H

#include "About_this_app.h"
#include "Defaults.h"

#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QMessageBox>

class ChooseFolderDialog : public QDialog {
Q_OBJECT

public:
    explicit
    inline
    ChooseFolderDialog(
            const QDir default_dir, // NOLINT Don't use a reference because the caller may change it. 
            QWidget *parent = nullptr):
            QDialog(parent),
            selected_path_label {new QLabel(default_dir.path(), this)}
    {
        // Makes no sense to emit signal inside the constructor because this instance can't be accessed yet from outside...
//        emit folder_set(selected_path_label->text());

//        setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

        auto* vertical_layout = new QVBoxLayout(this);
        
        auto* path_select_layout = new QHBoxLayout();
        
        auto* select_dir_button        = new QPushButton("Select a custom folder", this);
        auto* revert_to_default_button = new QPushButton("Revert to default", this);
        auto* ok_button = new QPushButton("OK", this);

        vertical_layout->addLayout(path_select_layout);
        
        path_select_layout->addWidget(selected_path_label);
        path_select_layout->addWidget(select_dir_button);
        path_select_layout->addWidget(revert_to_default_button);
        
        vertical_layout->addWidget(ok_button);
        
        connect(revert_to_default_button, &QPushButton::clicked,
                [this, default_dir]() {
                    selected_path_label->setText(default_dir.path());
                    emit folder_set(selected_path_label->text());
                }
        );

        connect(select_dir_button, &QPushButton::clicked,
            [this]() {
                const QString user_selection = QFileDialog::getExistingDirectory(nullptr,
                                                  "Select the directory, in which the note files will be scanned.",
                                                  selected_path_label->text(),
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks
                );
                
                if (not user_selection.isEmpty()) {
                    selected_path_label->setText(user_selection);
                    emit folder_set(selected_path_label->text());
                }
            }
        );

        connect(ok_button, &QPushButton::clicked,
                this, &ChooseFolderDialog::accept
        );
    }

signals:
    void folder_set(QString path); // NOLINT

private:
    QLabel* selected_path_label;
};

class Settings: public QSettings {
public:

    static
    inline
    /**
     *  parent cannot be nullptr
     */
    ChooseFolderDialog* get_user_default_folder() {

//        QSettings qsettings;

//        if (const QVariant value = qsettings.value(Keys::UserDefaultsGroup::notes_folder);
//                not QDir(value.toString()).exists()
//                ) {
            // Notes folder does not exist


//            auto create_default_dir = QMessageBox::question(nullptr,
//                                  "Folder creation",
//                                  "The default folder does not exist. Do you want to create it in the default path?",
//                                  QMessageBox::Yes | QMessageBox::No);

        QDir dir = vendor_defaults::notes_folder_qdir().path();
        auto* dialog = new ChooseFolderDialog{dir};

        // Application modal
        dialog->setModal(true);

        

//        bool quit_app = false;
//        while (true) {
//            quit_app = false;
//
//
//
//            // true if the user accepts the folder path
//            bool accepted = false;
//            connect(
//                    dialog, &ChooseFolderDialog::accepted,
//                    [&accepted]() {
//                        accepted = true;
//                    }
//            );
//
//            qDebug() <<"start dialog exec";
//            dialog->exec();
//            qDebug() <<"end dialog exec";
//
//            if (accepted) {
//                qDebug() <<"accepted";
//                break;
//            } else {
//                qDebug() <<"not accepted";
//                QWidget dummy; 
//                auto quit_app_confirmed = QMessageBox::question(&dummy, "Confirmation", "Do you want to close this application?",
//                                                                QMessageBox::Yes | QMessageBox::No);
//
//                if (quit_app_confirmed == QMessageBox::Yes) {
//                    quit_app = true;
//                    break;
//                }
//
//                // Don't quit app => ask again.
//            }
//        }

//        if (quit_app) {
//            QCoreApplication::quit();
//            return Error::quit_requested_by_user;
//        }

        // Create directory
//            QDir{}.mkdir(dir.path());
//            settings.setValue(Keys::UserDefaultsGroup::notes_folder, vendor_defaults::notes_folder_qdir().path());
//        }
        
        return dialog;
    }

    inline
    Settings() {
        get_user_default_folder();
    }
    
    struct Keys {

        struct UserDefaultsGroup {
            static inline const QString
                    group_name = "user_defaults";

            static inline const QString
                    notes_folder = group_name + "/" + "notes_folder_qdir";
        };
    }; 
    
};

#endif //SIMPLEMARKDOWN_SETTINGS_H
