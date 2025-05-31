# Минимальный пример .pro-файла для проекта с SQLite
QT       += core gui sql  # Добавлен модуль sql
QT += sql
TARGET   = MyNotesApp
TEMPLATE = app
SOURCES += main.cpp \
           MainWindow.cpp \
           ExpandableNoteButton.cpp
HEADERS += MainWindow.h \
           ExpandableNoteButton.h
LIBS += -lsqlite3
