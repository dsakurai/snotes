#include <QApplication>
#include <QSortFilterProxyModel>
#include "MainWindow.h"
#include "NotesListView.h"

#include <QStandardPaths>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    MainWindow mainWindow;

    qDebug() << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);


    auto* model = new NotesFolderModel{};
    
    QString path = "/Users/daisukesakurai/tmp/qtmarkdowntest/resources/test_markdown_directory";
    model->setRootPath(path);
    
    mainWindow.notesListView()->setProjectFolderModel(model);
    
    mainWindow.show();

    return QApplication::exec();
}
