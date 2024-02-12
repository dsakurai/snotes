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
        auto* layout = new QHBoxLayout();
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
    }
    
public slots:
    void open_file(const QString path) {
    
        if (markdownEditor) {
            layout()->removeWidget(markdownEditor);
            markdownEditor->deleteLater();
        }

        if (!QFile(path).exists()) return;

        markdownEditor = new SNotesMarkdownEditor(path, this);
        layout()->addWidget(markdownEditor);
    };

private:
    QPointer<SNotesMarkdownEditor> markdownEditor = nullptr;
};

#endif //SIMPLEMARKDOWN_NOTEVIEWSAREA_H
