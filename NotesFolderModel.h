//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
#define SIMPLEMARKDOWN_NOTESFOLDERMODEL_H

#include <QFileSystemModel>
#include <QSortFilterProxyModel>

class NotesFolderModel: public QSortFilterProxyModel {// TODO maybe QProxyModel is better?

    Q_OBJECT

public:
    explicit
    NotesFolderModel(QObject* parent = nullptr): QSortFilterProxyModel(parent) {
    
        auto* fileSystemModel = new QFileSystemModel(this);
        fileSystemModel->setNameFilters(QStringList("*.md"));
        fileSystemModel->setNameFilterDisables(false);
        fileSystemModel->setFilter(QDir::Files);

        setSourceModel(fileSystemModel);
    }

    void setSourceModel(QAbstractItemModel *sourceModel) override {
    
        if (this->sourceModel()) {
            throw std::runtime_error {"source model is already set."};
            // We don't allow this because we don't want to implement the logic to un-connect the signal-slots.
        }
    
        if (auto* fileSystemModel = dynamic_cast<QFileSystemModel*>(sourceModel)) {
            QSortFilterProxyModel::setSourceModel(fileSystemModel);
            
            connect(fileSystemModel, &QFileSystemModel::directoryLoaded,
                this, &NotesFolderModel::directoryLoaded
            );
        }
    }

    QFileSystemModel* sourceFileSystemModel() {
        auto* model =  sourceModel();
        return dynamic_cast<QFileSystemModel*>(model);
    }
    
    inline
    void setRootPath(const QString& path) {
        if(QFileSystemModel* model = sourceFileSystemModel()) {
            model->setRootPath(path);
        }
    }
    
    QString rootPath() {
    
        QString out {""};

        if(QFileSystemModel* model = sourceFileSystemModel()) {
            out = model->rootPath();
        }
        
        return out;
    }

    QString filePath(const QModelIndex &index) {
    
        QString out {""};
        
        if(QFileSystemModel* model = sourceFileSystemModel()) {
            out = model->filePath(index);
        }
        
        return out;
    }
    
    using QSortFilterProxyModel::index;

    QModelIndex index(const QString& path) {
        QModelIndex out;
        if(QFileSystemModel* model = sourceFileSystemModel()) {
            out = mapFromSource(
                    model->index(path)
            );
        }
        return out;
    }

signals:
    void directoryLoaded(const QString &path);
};

#endif //SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
