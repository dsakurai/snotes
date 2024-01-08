//
// Created by Daisuke Sakurai on 2024/01/08.
//

#ifndef SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
#define SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H

#include <qmarkdowntextedit.h>
#include <QFile>

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
        }
    };
    
    inline
    void save_file_immediately() {
    
        // No path set.
        if (!path) return;

        // save file

        if (QFile file (*path);
                file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QString text = this->document()->toRawText();
            file.write(text.toUtf8());
        }

        is_save_requested = false;
    }

protected slots:
    void request_save() { is_save_requested = true; };
    
private:
    bool is_save_requested = false;
    std::optional<QString> path;
};


#endif //SIMPLEMARKDOWN_SNOTESMARKDOWNEDITOR_H
