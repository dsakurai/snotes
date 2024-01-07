//
// Created by Daisuke Sakurai on 2024/01/06.
//

#ifndef SIMPLEMARKDOWN_DEFAULTS_H
#define SIMPLEMARKDOWN_DEFAULTS_H

#include <QDir>
#include <QStandardPaths>

namespace vendor_defaults {

    inline
    QString app_folder_name() {return "SNotes";}

    inline
    QString notes_folder_name() {return "notes";}

    inline
    QDir notes_folder_qdir() {
        QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString notes = documents            + QDir::separator()
                       + app_folder_name()  + QDir::separator()
                       + notes_folder_name()
        ;
        return QDir {notes};
    }
}

#endif //SIMPLEMARKDOWN_DEFAULTS_H
