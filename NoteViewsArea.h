//
// Created by Daisuke Sakurai on 2024/01/10.
//

#ifndef SIMPLEMARKDOWN_NOTEVIEWSAREA_H
#define SIMPLEMARKDOWN_NOTEVIEWSAREA_H

#include "SNotesMarkdownEditor.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QShortcut>

class NoteViewsArea;

class FindLineEdit: public QLineEdit {
Q_OBJECT
public:
    using QLineEdit::QLineEdit;
protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape)
                emit escapePressed();
        else
            QLineEdit::keyPressEvent(event);
    }

signals:
    void escapePressed();
};

class FindBox: public QWidget {
Q_OBJECT
public:
    FindBox(NoteViewsArea* parent);
    
    FindLineEdit* findLineEdit() {return findLineEdit_;}
    QTextEdit* editor();
    
private:
    FindLineEdit* findLineEdit_ = new FindLineEdit(this);
};

class NoteViewsArea: public QWidget {
Q_OBJECT

public:
    explicit
    NoteViewsArea(QWidget* parent = nullptr):
            QWidget(parent)
    {
        // set layout
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);

        findBox = new FindBox(this);
        layout->addWidget(findBox);
        findBox->hide();
    }
    
    QTextEdit* markdownEditor() {return markdownEditor_;}
    
public slots:
    void open_file(const QString path) {
    
        if (markdownEditor_) {
            layout()->removeWidget(markdownEditor_);
            markdownEditor_->deleteLater();
        }

        if (!QFile(path).exists()) return;

        markdownEditor_ = new SNotesMarkdownEditor(path, this);
        layout()->addWidget(markdownEditor_);
        
        // Hide the find box when closing the file.
        connect(markdownEditor_, &QTextEdit::destroyed,
                findBox, &QLineEdit::hide);

        // Zoom in / out (magnify font)
        new QShortcut(QKeySequence("Ctrl+="), this,
            [this]() {
                QTextDocument* doc = markdownEditor_->document();
                if (!doc) return;
                
                QFont font = doc->defaultFont();
                font.setPointSize(
                        font.pointSize() +1
                );
                doc->setDefaultFont(font);
            }
        );
        new QShortcut(QKeySequence::ZoomOut, this,
                      [this]() {
                          QTextDocument* doc = markdownEditor_->document();
                          if (!doc) return;

                          QFont font = doc->defaultFont();
                          font.setPointSize(
                                  font.pointSize() -1
                          );
                          doc->setDefaultFont(font);
                      }
        );
    };

private:
    QPointer<SNotesMarkdownEditor> markdownEditor_ = nullptr;
    FindBox* findBox = nullptr;
};

#endif //SIMPLEMARKDOWN_NOTEVIEWSAREA_H
