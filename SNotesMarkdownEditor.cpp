//
// Created by Daisuke Sakurai on 2024/01/08.
//

#include "SNotesMarkdownEditor.h"

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QShortcut>
#include <QUuid>
#include <Settings.h>

#include <QtConcurrent/QtConcurrent>

void IO::readAllNow() {
    is_load_requested = false;

    if (editor.isNull()) return;

    if (!path) {
        editor->deleteLater();
        return;
    }

    if (QFile file {*path};
            file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        
        const QString content = file.readAll(); 
        editor->setPlainText(std::move(content));

    } else {
        editor->deleteLater(); return;
    }
}

void IO::save_file_immediately() {

    is_save_requested = false;

    if (editor.isNull()) return;

    if (!path) return; // No path set.

    // The text in the editor
    const QString text = editor->document()->toPlainText(); // Don't use toRawText(). It will use the wrong line separator (maybe \r, I don't know which).
    
    QString file_path = *path;

    // Write the file in a new thread.
    auto _ = QtConcurrent::run(
        [=]() {
            QString original; // Original text in the file
            if (QFile file (file_path); file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                original = file.readAll();
            } else {
                // If we can't read from the file, we don't know if the content in the editor matches with the actual file contents we failed to read.
                // We therefore cancel the write and quit.
                /* Display warning */ QMessageBox::critical(nullptr, "Reading Fail", "Reading from the file failed. Check the file permissions.");
                return;
            }

            if (text == original) return; // no point writing the same text

            // Write the content to file
            if (QFile file (file_path); file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
                file.write(text.toUtf8());
            } else /* Display warning */ QMessageBox::critical(nullptr, "Writing Fail", "Writing to the file failed. Check the file permissions.");
        }
    );
}

void SNotesMarkdownEditor::keyPressEvent(QKeyEvent *event) {

    if (event->type() == QEvent::KeyPress) {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->matches(QKeySequence::Paste)) {
            if (mimeData->hasImage()) {
            
                const std::optional<QString> md_file_path = io->get_path();
                if (md_file_path) {
                    const QString parentFolderPath = QFileInfo(*md_file_path).absoluteDir().absolutePath();

                    const QDateTime currentDateTime = QDateTime::currentDateTime();
                    const QString str = currentDateTime.toString("yyyy-MM-dd-hh-mm-ss"); // ISO-like format

                    auto uuid = QUuid::createUuid().toString();
                    uuid = uuid.remove("{").remove("}");
                    const QImage img = qvariant_cast<QImage>(mimeData->imageData());
                    
                    const auto imagePath = "attachments/Clipboard-" + str + uuid + ".png";
                    img.save(parentFolderPath+ "/" + imagePath);

                    insertPlainText(QString("![](%1)\n").arg(imagePath));

                    return;
                }
            }
        }
    }
    QTextEdit::keyPressEvent(event);
}
