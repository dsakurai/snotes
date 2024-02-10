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

    void readAllNow();

    void save_file_immediately();

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
