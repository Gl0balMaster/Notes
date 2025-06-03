#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTimer>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void saveCurrentNote();
    void onTextChanged();

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QString m_currentNoteTitle;
    QTimer* m_saveTimer;
    bool m_isNoteLoading = false;
    int m_noteCounter = 0;

    void initDatabase();
    void loadNotes();
    void createNewNote();
    void addNoteToUI(const QString &title, const QString &content);
    void selectNote(const QString &title);
    void deleteNote(const QString &title, QVBoxLayout *layout);
};
#endif
