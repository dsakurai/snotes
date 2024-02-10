//
// Created by Daisuke Sakurai on 2024/01/08.
//

#ifndef SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
#define SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H

#include <qmarkdowntextedit.h>
#include <QFile>
#include <QMessageBox>
#include <QFileSystemWatcher>

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

class IO: public QObject {
    Q_OBJECT
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

        {
            auto* ask_to_save_timer = new QTimer(editor);
            QObject::connect(
                    ask_to_save_timer, &QTimer::timeout,
                    [this]() {
                        if (is_save_requested) save_file_immediately();
                    }
            );
            ask_to_save_timer->start(1000); // every second
        }

        {
            auto* ask_to_load_timer = new QTimer(editor);
            QObject::connect(
                    ask_to_load_timer, &QTimer::timeout,
                    [this]() {
                        if (is_load_requested) readAllNow();
                    }
            );
            ask_to_load_timer->start(500); // every 500msec
        }

        watcher.addPath(path);
        QObject::connect(
                &watcher, SIGNAL(fileChanged(QString)),
                this, SLOT(try_load(QString))
        );
    }

    void readAllNow();

    void save_file_immediately();

    void request_load() {is_load_requested = true;}
    void request_save() {is_save_requested = true;}

    ~IO() {
        if (path) save_file_immediately();
    }

public slots:
    void try_load(QString path) {
        // In case the file is replaced with a new one at the same path, we add the path again.
        if (!watcher.files().contains(path)) {
            watcher.addPath(path);
        }
        
        if (counter > 0) { // The editor saved the file.
            --counter;
        } else {
            request_load();
        }
    };

private:
    QFileSystemWatcher watcher;
    QPointer<SNotesMarkdownEditor> editor;
    std::optional<QString> path;
    int counter = 0;
    bool is_save_requested = false;
    bool is_load_requested = false;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
