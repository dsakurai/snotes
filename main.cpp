#include <QApplication>
#include <qmarkdowntextedit.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QMarkdownTextEdit markdownedit;
    markdownedit.setPlainText(
R"(
Test
=====

[test](google.co.jp)
)");
    
    markdownedit.show();
    
    return QApplication::exec();
}
