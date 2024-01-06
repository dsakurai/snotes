//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
#define SIMPLEMARKDOWN_NOTESFOLDERMODEL_H

#include <QFileSystemModel>

class NotesFolderModel: public QFileSystemModel {
    Q_OBJECT

public:
    explicit
    NotesFolderModel(QObject* parent = nullptr): QFileSystemModel(parent) {
        setNameFilters(QStringList("*.md"));
        setNameFilterDisables(false);
        setFilter(QDir::Files);
    }

    using QFileSystemModel::setRootPath;
    using QFileSystemModel::index;
};

#endif //SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
