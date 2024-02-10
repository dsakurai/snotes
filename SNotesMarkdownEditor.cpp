//
// Created by Daisuke Sakurai on 2024/01/08.
//

#include "SNotesMarkdownEditor.h"

void IO::readAllNow() {
    is_load_requested = false;

    if (editor.isNull()) return;

    if (!path) return;

    if (QFile file {*path};
            file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        editor->setPlainText(
                file.readAll());
    }
//     else {
//        /* Display warning */ QMessageBox::critical(nullptr, "Reading Fail", "Reading from the file failed. Check the file permissions.");
//        return;
//    }
}

void IO::save_file_immediately() {

    is_save_requested = false;

    if (editor.isNull()) return;

    if (!path) return; // No path set.

    QString original; // Original text in the file
    if (QFile file (*path); file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        original = file.readAll();
    } else {
        // If we can't read from the file, we don't know if the content in the editor matches with the actual file contents we failed to read.
        // We therefore cancel the write and quit.
        /* Display warning */ QMessageBox::critical(nullptr, "Reading Fail", "Reading from the file failed. Check the file permissions.");
        return;
    }

    // The text in the editor
    const QString text = editor->document()->toPlainText(); // Don't use toRawText(). It will use the wrong line separator (maybe \r, I don't know which).
    if (text == original) return; // no point writing the same text

    // Write the content to file
    if (QFile file (*path); file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
        file.write(text.toUtf8());
    } else /* Display warning */ QMessageBox::critical(nullptr, "Writing Fail", "Writing to the file failed. Check the file permissions.");
}
