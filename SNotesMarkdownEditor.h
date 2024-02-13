//
// Created by Daisuke Sakurai on 2024/01/08.
//

#ifndef SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
#define SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H

#include <qmarkdowntextedit.h>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextEdit>
#include <QFileSystemWatcher>

class IO;

/**
 * This editor is tasked with editing only one file. It will not be re-used for editing a different file.
 * This design is chosen to make sure that this editor will not write the content of one file to another, when failing 
 * to switch the state properly.
 * 
 * An instance of this class can be deleted from the IO class, in case of file read error. 
 */
class SNotesMarkdownEditor: public QTextEdit {
    Q_OBJECT
    
public:

    std::unique_ptr<IO> io;

    explicit
    SNotesMarkdownEditor(QString path, QWidget* parent = nullptr):
            QTextEdit(parent)
    {
        // This highlights the text in this editor.
        new MarkdownHighlighter(this->document());
        
        // Rich text not allowed
        this->setAcceptRichText(false);

        // Do this inside the constructor (not in the initializer list) because IO might call methods of SNotesMarkdownEditor.
        io = std::make_unique<IO>(this, path);
    }
    
public:
    
private:
};

/**
 * Handle the IO operation of an editor.
 * An IO instance's lifetime is managed by the editor.
 * Do not use QObject's lifetime handling.
 */
class IO: public QObject {
    Q_OBJECT
public:
    IO(const QPointer<SNotesMarkdownEditor>& editor, QString path):
            editor {editor},
            path {path}
    {
        if (!QFile(path).exists()) {
            editor->deleteLater();
            return;
        }
    
        readAllNow();

        QObject::connect(
                editor.get(), &SNotesMarkdownEditor::textChanged,
                [this]() {
                    timestamps[*this->path] = QDateTime::currentDateTime();
                }
        );

        QObject::connect(
                editor.get(), &SNotesMarkdownEditor::textChanged,
                [this]() {
                    request_save();
                }
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
            ask_to_load_timer->start(100); // every few hundred msecs
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
        if (path && QFile(*path).exists()) save_file_immediately();
    }

public slots:
    void try_load(QString path) {

        if (!QFile(path).exists()) {
            editor->deleteLater();
            return;
        }
    
        // In case the file is replaced with a new one at the same path, we add the path again.
        if (!watcher.files().contains(path)) {
            qDebug() << "path: " << path;
            watcher.addPath(path);
        }

        if (editor.isNull()) return;
        
        const QDateTime lastModified = QFileInfo(path).lastModified();
        if (lastModified < timestamps[path]) return;
        
        if (QFile file {path};
                file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = file.readAll();
            if (content != editor->toPlainText()) request_load();
        }
    };

private:
    QFileSystemWatcher watcher;
    QPointer<SNotesMarkdownEditor> editor;
    std::optional<QString> path;
    bool is_save_requested = false;
    bool is_load_requested = false;
    static inline std::map<QString, QDateTime> timestamps;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
