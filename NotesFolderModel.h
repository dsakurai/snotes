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
    NotesFolderModel(QObject* parent = nullptr): QSortFilterProxyModel(parent) {}

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

private:
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
    
    void setSortOrder(Qt::SortOrder order);

signals:
    void directoryLoaded(const QString &path);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    Qt::SortOrder sortOrder = Qt::SortOrder::AscendingOrder;
};

#endif //SIMPLEMARKDOWN_NOTESFOLDERMODEL_H
