//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
#define SIMPLEMARKDOWN_NOTESFOLDERMODEL_H

#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <set>

class NotesFolderModel: public QSortFilterProxyModel {// TODO maybe QProxyModel is better?

    Q_OBJECT

public:
    explicit
    NotesFolderModel(QObject* parent = nullptr): QSortFilterProxyModel(parent) {}

    void setSourceModel(QFileSystemModel *sourceModel) {
    
        if (sourceFileSystemModel() && (sourceFileSystemModel()->rootPath() != sourceModel->rootPath())) {
            throw std::runtime_error {"Root path is changed."};
        }
    
        if (auto* fileSystemModel = dynamic_cast<QFileSystemModel*>(sourceModel)) {
            update_pinned_files_set(fileSystemModel);

            QSortFilterProxyModel::setSourceModel(fileSystemModel);
        }
    }
    

private:

    void update_pinned_files_set(QFileSystemModel* fileSystemModel);
    
    QFileSystemModel* sourceFileSystemModel() {
        auto* model =  sourceModel();
        return dynamic_cast<QFileSystemModel*>(model);
    }

    [[nodiscard]]
    const QFileSystemModel* sourceFileSystemModel() const {
        auto* model =  sourceModel();
        return dynamic_cast<const QFileSystemModel*>(model);
    }

public:

    bool filterFile(int source_row, const QModelIndex &source_parent) const;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        QString root_path = sourceFileSystemModel()->rootPath();
        root_path = QDir(root_path).canonicalPath();
        root_path = QDir::cleanPath(root_path);
        
        QString parent_path = filePath(source_parent);
        parent_path = QDir(parent_path).canonicalPath();
        parent_path = QDir::cleanPath(parent_path);

        // offspring
        if (parent_path.startsWith(root_path))
            return filterFile(source_row, source_parent);

        // ancestor
        return true; // Always accept ancestors
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

    QString filePath(const QModelIndex &index) const {
    
        QString out {""};
        
        if(const QFileSystemModel* source_model = sourceFileSystemModel()) {

            QModelIndex index_for_source = index;
            if (index.model() == this) index_for_source = mapToSource(index); 

            out = source_model->filePath(
                    index_for_source
            );
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
    
protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
public:
    std::set<QString> pinned_files_set;
};

#endif //SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
