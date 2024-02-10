//
// Created by Daisuke Sakurai on 2024/01/08.
//

#ifndef SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
#define SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H

#include <qmarkdowntextedit.h>
#include <QFile>
#include <QMessageBox>

class IO;

class SNotesMarkdownEditor: public QMarkdownTextEdit {
    Q_OBJECT
    
public:

    std::unique_ptr<IO> io;

    explicit
    SNotesMarkdownEditor(QString path, QWidget* parent = nullptr):
            QMarkdownTextEdit(parent)
    {
        // Do this inside the constructor (not in the initializer list) because IO might call methods of SNotesMarkdownEditor.
        io = std::make_unique<IO>(this, path);
    }
    
public:
    
private:
};

class IO {
public:
    IO(const QPointer<SNotesMarkdownEditor>& editor, QString path):
            editor {editor},
            path {path}
    {
        readAllNow();

        QObject::connect(
                editor.get(), &SNotesMarkdownEditor::textChanged,
                [this]() { request_save(); }
        );

        auto* ask_to_save_timer = new QTimer(editor);
        QObject::connect(
                ask_to_save_timer, &QTimer::timeout,
                [this]() {
                    if (is_save_requested) save_file_immediately();
                }
        );

        QObject::connect(
                editor.get(), &SNotesMarkdownEditor::textChanged,
                [this, ask_to_save_timer=ask_to_save_timer]() {
                    ask_to_save_timer->start(1000); // every second
                }
        );
    }

    void readAllNow() {

        if (editor.isNull()) return;

        if (!path) return;

        if (QFile file {*path};
                file.open(QIODevice::ReadOnly | QIODevice::Text)) {

            editor->setPlainText(
                    file.readAll());

        } else {
            /* Display warning */ QMessageBox::critical(nullptr, "Reading Fail", "Reading from the file failed. Check the file permissions.");
            return;
        }
    }

    inline
    void save_file_immediately() {

        qDebug() << "saving file";

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

    void request_save() {is_save_requested = true;}

    ~IO() {
        if (path) save_file_immediately();
    }

private:
    QPointer<SNotesMarkdownEditor> editor;
    std::optional<QString> path;
    bool is_save_requested = false;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
