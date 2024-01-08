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

    explicit
    SNotesMarkdownEditor(QWidget* parent = nullptr): QMarkdownTextEdit(parent) {
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
    void open_file(const QString path){
        if (this->path) save_file_immediately();
    
        this->path = path;
        
        if (QFile file (path);
                file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString text = file.readAll();
            setPlainText(text);
        } else {
            /* Display warning */ QMessageBox::critical(this, "Reading Fail", "Reading from the file failed. Check the file permissions.");
        }
    };
    
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
        const QString text = this->document()->toRawText();
        if (text == original) return; // no point writing the same text
        
        // Write the content to file
        if (QFile file (*path); file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
            file.write(text.toUtf8());
        } else /* Display warning */ QMessageBox::critical(this, "Writing Fail", "Writing to the file failed. Check the file permissions.");
    }

protected slots:
    void request_save() { is_save_requested = true; };
    
private:
    bool is_save_requested = false;
    std::optional<QString> path;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
