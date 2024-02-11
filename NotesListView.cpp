//
// Created by Daisuke Sakurai on 2024/02/11.
//

#include "NotesListView.h"
#include <ranges>

#include <QMenu>
#include <QProcess>

void NotesListView::reveal_in_folder() {

    const auto* selectionModel = this->selectionModel();
    if (!selectionModel) return;
    const NotesFolderModel* files = model();
    if (!files) return;

    auto to_file_path = std::ranges::views::transform(
        [&files](const QModelIndex& index) -> QString {
            return
                    QDir::toNativeSeparators( // Use native path separator
                            QDir::cleanPath(  // Resolve ".." etc.
                                    files->filePath(index)));
        }
    );
    
    auto file_paths = selectionModel->selectedRows() | to_file_path;
    
    QString script = QString(R"(
        on run argv
            set posixFileList to {}

            repeat with aPath in argv
                set end of posixFileList to POSIX file aPath as string
            end repeat

            tell application "Finder"
                activate
                reveal posixFileList
            end tell
        end run)");
    QProcess::startDetached("osascript",
                            QStringList() << "-e" << script
                                          << QStringList{file_paths.begin(), file_paths.end()});
}

NotesListView::NotesListView(QWidget *parent) : QTreeView(parent) {

    // Context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested,
            [this](const QPoint &point) {
                QMenu contextMenu (tr("Context Menu"), this);
                
                auto* reveal = new QAction("Reveal Folder", this);
                connect(reveal, &QAction::triggered, this, &NotesListView::reveal_in_folder);
                contextMenu.addAction(reveal);

                contextMenu.exec(mapToGlobal(point));
            });

}
