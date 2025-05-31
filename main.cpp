#include "MainWindow.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        QMessageBox::critical(nullptr, "Error", "SQLite driver not available!");
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
