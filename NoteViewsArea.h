//
// Created by Daisuke Sakurai on 2024/01/10.
//

#ifndef SIMPLEMARKDOWN_NOTEVIEWSAREA_H
#define SIMPLEMARKDOWN_NOTEVIEWSAREA_H

#include "SNotesMarkdownEditor.h"

#include <QWidget>
#include <QHBoxLayout>

class NoteViewsArea: public QWidget {
Q_OBJECT

public:
    explicit 
    NoteViewsArea(QWidget* parent = nullptr): QWidget(parent) {
        // set layout
        new QHBoxLayout(this);
    }
    
public slots:
    void open_editor(QString path) {
    
        auto create_new_editor = [this, path]() {
            markdownEditor = new SNotesMarkdownEditor(this);
            this->layout()->addWidget(markdownEditor);
            markdownEditor->open_file(path);
        };
    
        if(markdownEditor) { // an editor widget is already there
            markdownEditor->deleteLater(); // delete the current one
            connect(markdownEditor, &SNotesMarkdownEditor::destroyed, // on desrtruction
                    this, create_new_editor // create a new editor
            );
        } else {
            create_new_editor(); // create a new editor immediately
        }
    };

private:
    QPointer<SNotesMarkdownEditor> markdownEditor = nullptr;
};

#endif //SIMPLEMARKDOWN_NOTEVIEWSAREA_H
