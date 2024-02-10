//
// Created by Daisuke Sakurai on 2024/01/08.
//

#ifndef SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
#define SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H

#include <qmarkdowntextedit.h>
#include <QFile>
#include <QMessageBox>

class SNotesMarkdownEditor: public QMarkdownTextEdit {
    Q_OBJECT
    
public:

    class IO {
    public:
        IO(const QPointer<SNotesMarkdownEditor>& editor): editor {editor}
         {}
         
         QString readAllNow() {

             if (editor.isNull()) return {};

             auto path = editor->getPath();
             if (!path) return {};
             
             if (QFile file {*path};
                     file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                 return file.readAll();
             } else {
                 /* Display warning */ QMessageBox::critical(nullptr, "Reading Fail", "Reading from the file failed. Check the file permissions.");
                 return {};
             }
         }
         
         QPointer<SNotesMarkdownEditor> editor;
    };
    std::unique_ptr<IO> io;

    explicit
    SNotesMarkdownEditor(QString path, QWidget* parent = nullptr):
            QMarkdownTextEdit(parent),
            io {std::make_unique<IO>(this)},
            path {path}
    {
        
        setPlainText(
                io->readAllNow()
        );
        
        connect(
            this, &SNotesMarkdownEditor::textChanged,
            this, &SNotesMarkdownEditor::request_save
        );
        
        auto* ask_to_save_timer = new QTimer(this);
        connect(
                ask_to_save_timer, &QTimer::timeout,
                [this]() {
                    if (is_save_requested) save_file_immediately();
                }
        );
        ask_to_save_timer->start(1000); // every second
    }

    ~SNotesMarkdownEditor() override {
        if (path) save_file_immediately();
    }
    
public slots:
    
    inline
    void save_file_immediately() {
    
        is_save_requested = false;
    
        if (!path) return; // No path set.
        
        QString original; // Original text in the file
        if (QFile file (*path); file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            original = file.readAll();
        } else {
            // If we can't read from the file, we don't know if the content in the editor matches with the actual file contents we failed to read.
            // We therefore cancel the write and quit.
            /* Display warning */ QMessageBox::critical(this, "Reading Fail", "Reading from the file failed. Check the file permissions.");
            return;
        }

        // The text in the editor
        const QString text = this->document()->toPlainText(); // Don't use toRawText(). It will use the wrong line separator (maybe \r, I don't know which).
        if (text == original) return; // no point writing the same text
        
        // Write the content to file
        if (QFile file (*path); file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
            file.write(text.toUtf8());
        } else /* Display warning */ QMessageBox::critical(this, "Writing Fail", "Writing to the file failed. Check the file permissions.");
    }

protected slots:
    void request_save() { is_save_requested = true; };


public:
    const std::optional<QString> &getPath() const {
        return path;
    }
    
private:
    bool is_save_requested = false;
    std::optional<QString> path;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
