//
// Created by Daisuke Sakurai on 2024/02/11.
//

#include "NotesListView.h"
#include <ranges>
#include <QProcess>

void NotesListView::reveal_in_folder() {

    const auto* selectionModel = this->selectionModel();
    if (!selectionModel) return;
    const NotesFolderModel* files = model();
    if (!files) return;

    auto to_file_path = std::ranges::views::transform(
        [&files](const QModelIndex& index) -> QString {
            return QString(R"( POSIX file "%1" )").arg(files->filePath(index));
        }
    );
    
    auto file_paths = selectionModel->selectedRows() | to_file_path;
    
    QString files_str = QStringList{file_paths.begin(), file_paths.end()}.join(", "); 
    
    QString script = QString(R"(
tell application "Finder"
    activate
    reveal {%1}
end tell
)").arg(files_str);
    QProcess::startDetached("osascript", QStringList() << "-e" << script);
}
