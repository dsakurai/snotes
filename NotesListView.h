//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_NOTESLISTVIEW_H
#define SIMPLEMARKDOWN_NOTESLISTVIEW_H

#include <QTreeView>
#include "NotesFolderModel.h"

class NotesListView: public QTreeView {
    Q_OBJECT

public:

    using QTreeView::QTreeView;

    void setProjectFolderModel(NotesFolderModel *model) {
        QTreeView::setModel(model);
        setRootIndex(model->index(model->rootPath()));

        sortByColumn(3, Qt::SortOrder::AscendingOrder); // Sort by the file modification date
        setSortingEnabled(true); // For performance, call this after setting the model.

        // TODO Check the column names instead of relying on these magic numbers
        hideColumn(1); // file size column
        hideColumn(2); // file type column
    }

    NotesFolderModel* model() { // NOLINT overriding a method that's actually not virtual
        auto* out = qobject_cast<NotesFolderModel*>(QTreeView::model());
        if (!out) {
            qFatal("Failed to access file system model");
        }
        return out;
    }
    
signals:
    void singleFileSelected(QString path); //NOLINT signal has not implementation
    
protected:
    inline
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override {
        const auto num_columns = model()->columnCount();
        
        if (selected.indexes().size() % num_columns != 0) qFatal("User can somehow select a part of a column. This was not anticipated when calculating the number of rows selected.");
        
        if (selected.indexes().size() / num_columns == 1) {
            const QModelIndex file = selected.indexes()[0];
            auto* files = model();
            if (!files) throw std::runtime_error {"Failed to access QFileSystemModel"};
            QString path = files->filePath(file);
            emit singleFileSelected(path);
        }

        QTreeView::selectionChanged(selected, deselected);
    }

private:
    // Do not use
    void setModel(QAbstractItemModel*) final {}
};


#endif //SIMPLEMARKDOWN_NOTESLISTVIEW_H
