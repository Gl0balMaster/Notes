#include "mainwindow.h"
#include "ExpandableNoteButton.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlQuery>
#include <QMessageBox>
#include <QTimer>
#include <qapplication.h>
#include <qsqlerror.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_centralWidget(new QWidget(this)),
    m_mainLayout(new QVBoxLayout(m_centralWidget))
{
    setCentralWidget(m_centralWidget);
    m_mainLayout->setAlignment(Qt::AlignTop);
    setMinimumSize(400, 300);

    if (!initDatabase()) {
        QMessageBox::warning(this, "Warning", "Working in offline mode - notes won't be saved!");
    }

    cleanDatabase();
    loadNotes();

    QPushButton *addButton = new QPushButton("Добавить заметку", this);
    m_mainLayout->addWidget(addButton);
    connect(addButton, &QPushButton::clicked, this, [this]() { addNewButton(); });
}

MainWindow::~MainWindow() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool MainWindow::initDatabase() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("notes.db");

    if (!m_db.open()) {
        qCritical() << "Database error:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS notes ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "title TEXT NOT NULL UNIQUE, "
                    "content TEXT NOT NULL, "
                    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)")) {
        qCritical() << "Table creation error:" << query.lastError().text();
        return false;
    }

    m_dbIsValid = true;
    return true;
}

void MainWindow::cleanDatabase() {
    if (!m_dbIsValid) return;

    QSqlQuery query;
    if (!query.exec("DELETE FROM notes")) {
        qCritical() << "Failed to clean database:" << query.lastError().text();
    }
    query.exec("DELETE FROM sqlite_sequence WHERE name='notes'");
    m_counter = 0;
}

void MainWindow::loadNotes() {
    if (!m_dbIsValid) return;

    QSqlQuery query("SELECT title, content FROM notes ORDER BY created_at DESC");
    while (query.next()) {
        addNewButton(query.value(0).toString(), query.value(1).toString());
    }
}

void MainWindow::addNewButton(const QString& title, const QString& content) {
    QString buttonTitle = title.isEmpty() ?
                              "Заметка " + QString::number(++m_counter) :
                              title;

    NoteBtn* newButton = new NoteBtn(buttonTitle, m_centralWidget);
    m_mainLayout->addWidget(newButton);
    m_mainLayout->addWidget(newButton->noteEditor());

    if (!content.isEmpty()) {
        newButton->noteEditor()->setText(content);
    }

    if (m_dbIsValid) {
        saveNoteToDatabase(buttonTitle, newButton->noteEditor()->toPlainText());
    }

    connect(newButton->noteEditor(), &QTextEdit::textChanged, [this, newButton]() {
        if (m_dbIsValid) {
            saveNoteToDatabase(newButton->text(), newButton->noteEditor()->toPlainText());
        }
    });

    connect(newButton, &NoteBtn::clicked, [this, newButton]() {
        if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
            if (m_dbIsValid) {
                deleteNoteFromDatabase(newButton->text());
            }
            newButton->deleteLater();
            QTimer::singleShot(0, this, [this]() { adjustSize(); });
        }
    });

    newButton->installEventFilter(this);
}

void MainWindow::saveNoteToDatabase(const QString& title, const QString& content) {
    if (!m_dbIsValid) return;

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO notes (title, content) VALUES (?, ?)");
    query.addBindValue(title);
    query.addBindValue(content);

    if (!query.exec()) {
        qCritical() << "Save error:" << query.lastError().text();
    }
}

void MainWindow::deleteNoteFromDatabase(const QString& title) {
    if (!m_dbIsValid) return;

    QSqlQuery query;
    query.prepare("DELETE FROM notes WHERE title = ?");
    query.addBindValue(title);

    if (!query.exec()) {
        qCritical() << "Delete error:" << query.lastError().text();
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if(event->type() == QEvent::MouseButtonDblClick) {
        if(auto* btn = qobject_cast<NoteBtn*>(obj)) {
            editNoteTitle(btn);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::editNoteTitle(NoteBtn* btn) {
    QString oldTitle = btn->text();

    QLineEdit* edit = new QLineEdit(btn->parentWidget());
    edit->setText(oldTitle);
    edit->setGeometry(btn->geometry());
    edit->show();
    btn->hide();

    auto finishEditing = [=]() {
        QString newTitle = edit->text().trimmed();
        if(!newTitle.isEmpty() && newTitle != oldTitle) {
            btn->setText(newTitle);
            if(m_dbIsValid) {
                updateNoteTitleInDatabase(oldTitle, newTitle);
            }
            emit btn->titleChanged(newTitle);
        }
        edit->deleteLater();
        btn->show();
        adjustSize();
    };

    connect(edit, &QLineEdit::editingFinished, finishEditing);
    connect(edit, &QLineEdit::returnPressed, finishEditing);
}

void MainWindow::updateNoteTitleInDatabase(const QString& oldTitle, const QString& newTitle) {
    if(!m_dbIsValid) return;

    m_db.transaction();

    QSqlQuery query;
    query.prepare("UPDATE notes SET title = ? WHERE title = ?");
    query.addBindValue(newTitle);
    query.addBindValue(oldTitle);

    if(!query.exec()) {
        m_db.rollback();
        qCritical() << "Title update failed:" << query.lastError();
    } else {
        m_db.commit();
    }
}
