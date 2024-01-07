//
// Created by Daisuke Sakurai on 2024/01/05.
//

#ifndef SIMPLEMARKDOWN_MAINWINDOW_H
#define SIMPLEMARKDOWN_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class NotesListView;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    
    NotesListView* notesListView();

    ~MainWindow() override;
    
signals:
    /**
     * Quit is requested from within the constructor of this class.
     */
    void quit_from_constructor(); // NOLINT

private:
    Ui::MainWindow *ui;
};


#endif //SIMPLEMARKDOWN_MAINWINDOW_H
