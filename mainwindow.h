#pragma once
#include <QMainWindow>
#include <qsqldatabase.h>

class QVBoxLayout;
class NoteBtn;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool initDatabase();
    void cleanDatabase();
    void loadNotes();
    void addNewButton(const QString& title = "", const QString& content = "");
    void updateNoteTitleInDatabase(const QString& oldTitle, const QString& newTitle);
    void saveNoteToDatabase(const QString& title, const QString& content);
    void deleteNoteFromDatabase(const QString& title);
    void editNoteTitle(NoteBtn* btn);

    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    int m_counter = 0;
    bool m_dbIsValid = false;
    QSqlDatabase m_db;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};
