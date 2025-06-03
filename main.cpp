#include "mainwindow.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>

const QString DbName = "notes.db";
const QString DbConnectionName = "notes_connection";


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    auto dbPath = QDir::currentPath() + QDir::separator()+ DbName;
    qDebug() << "path"<<dbPath;
    auto db = QSqlDatabase::addDatabase("QSQLITE", DbConnectionName);
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qCritical() << "Cannot open database:" << db.lastError().text();
        return -1;
    }

    // Создаем таблицу, если не существует
    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS notes ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "title TEXT NOT NULL, "
                    "content TEXT)")) {
        qCritical() << "Failed to create table:" << query.lastError().text();
    }

    w.show();
    return a.exec();
}
