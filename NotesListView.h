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
        QTreeView::setModel(dynamic_cast<QFileSystemModel*>(model));
        setRootIndex(model->index(model->rootPath()));
    }

private:
    // Do not use
    void setModel(QAbstractItemModel*) final {}
};


#endif //SIMPLEMARKDOWN_NOTESLISTVIEW_H
