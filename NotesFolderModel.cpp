//
// Created by Daisuke Sakurai on 2024/02/10.
//

#include <ryml.hpp>

#include "NotesFolderModel.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <fstream>

bool is_pinned(const std::string& path) {

    // These are re-allocated...
    // Better, use an object pool.
    std::string line;
    std::string yaml_header;

    if (std::ifstream stream {path};
            stream.is_open() ) { // I could read the file in binary mode for performance, maybe?

        std::getline(stream, line);

        if (yaml_header.size() > 1024) std::string{}.swap(yaml_header); // Free RAM

        // yaml header?
        if (line != "---") return false; // no

        bool found = false;
        while (std::getline(stream, line)) {

            if (line == "---") {
                found = true;
                break;
            }

            yaml_header += line + "\n";
        }

        if(!found) return false;

        ryml::Tree tree = ryml::parse_in_place(ryml::substr(
                yaml_header.data(), yaml_header.size()));

        auto pinned = tree["pinned"];
        if (pinned.valid() && pinned.has_val() ) {
            auto val = pinned.val();
            if ((val == "true") || (val == "yes")) {
                return true;
            }
        }

    } else {
        qDebug() << "Failed to read file.";
    }

    return false;
}

bool is_pinned(const QModelIndex &index, const NotesFolderModel& model) {
    QString path = model.filePath(index);
    return is_pinned(path.toStdString());
}

bool NotesFolderModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {

    // equal => false
    if (source_left == source_right) return QSortFilterProxyModel::lessThan(source_left, source_right);
    

    auto* filesystemmodel = this->sourceFileSystemModel();
    
    const QString path_left  = filesystemmodel->filePath(source_left);
    const QString path_right = filesystemmodel->filePath(source_right);

    const bool is_pinned_left  = pinned_files_set.find(path_left) != pinned_files_set.end();
    const bool is_pinned_right = pinned_files_set.find(path_right) != pinned_files_set.end();

    // only one is pinned
    if (is_pinned_left ^ is_pinned_right) {
        const bool pinned_is_less =  is_pinned_left && !is_pinned_right;
        // left is pinned => left is less
        return (sortOrder == Qt::AscendingOrder)? pinned_is_less : !pinned_is_less; // NOLINT
    }

    std::filesystem::file_time_type tl, tr;
    try {
        std::filesystem::file_time_type l;
        std::string s = filesystemmodel->filePath(source_left).toStdString();
        tl = std::filesystem::last_write_time(s);
    } catch (const std::filesystem::filesystem_error& err) {
        std::cerr << "Error: " << err.what() << '\n';
    }
    try {
        std::filesystem::file_time_type r;
        std::string s = filesystemmodel->filePath(source_right).toStdString();
        tr = std::filesystem::last_write_time(s);
    } catch (const std::filesystem::filesystem_error& err) {
        std::cerr << "Error: " << err.what() << '\n';
    }
    
    return tl < tr;

//    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

void NotesFolderModel::setSortOrder(Qt::SortOrder order) {
    this->sortOrder = order;
}

bool NotesFolderModel::filterFile(int source_row, const QModelIndex &source_parent) const {

    if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent)) {
        // file survives the default filtering
        return true;
    }
    
    // File is to be filtered away since the title does not match the query.
    // Check the file contents now...

    auto* fileSystemModel = sourceFileSystemModel();

    // check file contents
    QModelIndex index = fileSystemModel->index(source_row, 0, source_parent);
    QString filePath = fileSystemModel->filePath(index);

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false; // File couldn't be opened

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    // Better, allow multiple words rather than using fixed string...
    // I should not use QSortFilterProxyModel::filterAcceptsRow.
    
    auto re = filterRegularExpression();
    re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    return re.match(content).hasMatch();
}

void NotesFolderModel::update_pinned_files_set(QFileSystemModel* fileSystemModel) {

    pinned_files_set.clear();

    const QModelIndex root = fileSystemModel->index(fileSystemModel->rootPath());

    const int rows = fileSystemModel->rowCount(root);

    for (int r = 0; r < rows; ++r) {
        const QModelIndex child = fileSystemModel->index(r, 0, root);
        const QString path = fileSystemModel->filePath(child);
        if (is_pinned(path.toStdString())) {
            pinned_files_set.insert(path);
        }
    }

}
