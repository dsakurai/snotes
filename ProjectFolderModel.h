//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_PROJECTFOLDERMODEL_H
#define SIMPLEMARKDOWN_PROJECTFOLDERMODEL_H

#include <QFileSystemModel>

class ProjectFolderModel: public QFileSystemModel {
    Q_OBJECT

public:
    explicit
    ProjectFolderModel(QObject* parent = nullptr): QFileSystemModel(parent) {
        setNameFilters(QStringList("*.md"));
        setNameFilterDisables(false);
        setFilter(QDir::Files);
    }

    using QFileSystemModel::setRootPath;
    using QFileSystemModel::index;
};

#endif //SIMPLEMARKDOWN_PROJECTFOLDERMODEL_H
