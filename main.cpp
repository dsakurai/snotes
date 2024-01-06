#include <QApplication>
#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include "MainWindow.h"
#include "NotesListView.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    MainWindow mainWindow;

    auto* model = new ProjectFolderModel{};
    
    QString path = "/Users/daisukesakurai/tmp/qtmarkdowntest/resources/test_markdown_directory";
    model->setRootPath(path);
    
    mainWindow.notesListView()->setProjectFolderModel(model);
    
    mainWindow.show();

    return QApplication::exec();
}
