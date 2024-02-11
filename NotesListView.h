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

    NotesListView(QWidget* parent = nullptr);

    void setProjectFolderModel(NotesFolderModel *model) {
        QTreeView::setModel(model);
        setRootIndex(model->index(model->rootPath()));

        sortByColumn(3, Qt::SortOrder::DescendingOrder); // Sort by the file modification date
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
    
public slots:
    void reveal_in_folder();
    void customContextmenu(const QPoint& point);
    
protected:

    // This gets called:
    // 1. when the selected items have changed
    // 2. when the list of selectable items have changed
    // To see this, you can check it by printing the selected and de-selected items.
    // In case 2., both selected and deselected are 0.
    // qDebug() << "selected: " << selected.size() << "deselected: " << deselected.size();
    inline
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override {

        const NotesFolderModel* files = model();
        const auto* selectionModel = this->selectionModel();
        
        if (selected.size() > 0) { // Selection added
            if (files && selectionModel && selectionModel->selectedRows().size() == 1) {
                // Why do we get non-existent path?
                QString path = "";
                for (QModelIndex file: selectionModel->selectedRows()) {
                    path = files->filePath(file);
                }
                if (QFile(path).exists()) emit singleFileSelected(path);
            }
        }

        QTreeView::selectionChanged(selected, deselected);
    }

private:
    // Do not use
    void setModel(QAbstractItemModel*) final {}
};


#endif //SIMPLEMARKDOWN_NOTESLISTVIEW_H
