//
// Created by Daisuke Sakurai on 2024/02/12.
//

#include <NoteViewsArea.h>

#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QShortcut>

FindBox::FindBox(NoteViewsArea *parent):
        QWidget(parent)
{
    setLayout(new QHBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);

    layout()->addWidget(findLineEdit_);
    auto* findNextButton = new QPushButton("Next", this);
    layout()->addWidget(findNextButton);
    auto* findPreviousButton = new QPushButton("Previous", this);
    layout()->addWidget(findPreviousButton);

    new QShortcut(QKeySequence::Find, parent,
                  [this]() {
                      show();
                      findLineEdit()->setFocus();
                  }
    );
    
    connect(findLineEdit_, &FindLineEdit::escapePressed,
            this, &QWidget::hide
    );
    connect(findLineEdit_, &QLineEdit::returnPressed,
            [findNextButton=findNextButton, findPreviousButton=findPreviousButton]() {
                // Shift is pressed?
                if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
                    findPreviousButton->click(); // previous;
                } else {
                    findNextButton->click(); // next
                }
            }
    );

    connect(findNextButton, &QPushButton::clicked,
            [this]() {
                if (QTextEdit* ed = editor()) ed->find(findLineEdit_->text());
            }
    );
    connect(findPreviousButton, &QPushButton::clicked,
            [this]() {
                // mark down editor may change, so we need to ask its address to noteViewsArea. 
                if (auto* ed = editor()) ed->find(findLineEdit_->text(),QTextDocument::FindFlag::FindBackward);
            }
    );
}

QTextEdit *FindBox::editor() {
    // markdownEditor may be destroyed, so we need to ask its address to noteViewsArea. 
    auto* noteViewsArea = qobject_cast<NoteViewsArea*>(parentWidget());
    if (!noteViewsArea) return nullptr;

    return noteViewsArea->markdownEditor();
}
