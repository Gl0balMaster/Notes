#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_db(QSqlDatabase::addDatabase("QSQLITE"))
    , m_saveTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Создаем контейнерный виджет для заметок
    QWidget *notesContainer = new QWidget();
    m_notesLayout = new QVBoxLayout(notesContainer); // Теперь это член класса
    m_notesLayout->setAlignment(Qt::AlignTop);
    notesContainer->setLayout(m_notesLayout);

    // Настраиваем scrollArea
    ui->scrollArea->setWidget(notesContainer);
    ui->scrollArea->setWidgetResizable(true);

    // Инициализация базы данных
    initDatabase();

    // Настройка таймера автосохранения
    m_saveTimer->setSingleShot(true);
    connect(m_saveTimer, &QTimer::timeout, this, &MainWindow::saveCurrentNote);

    // Подключение сигналов
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::createNewNote);
    connect(ui->titleEdit, &QLineEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->contentEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);

    // Загрузка заметок
    loadNotes();
}

void MainWindow::initDatabase()
{
    m_db.setDatabaseName("notes.db");
    if (!m_db.open()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS notes ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "title TEXT UNIQUE NOT NULL, "
               "content TEXT NOT NULL)");
}

void MainWindow::loadNotes()
{
    QSqlQuery query("SELECT title, content FROM notes ORDER BY id");
    while (query.next()) {
        addNoteToUI(query.value(0).toString(), query.value(1).toString());
    }
}

void MainWindow::addNoteToUI(const QString &title, const QString &content)
{
    QVBoxLayout *noteLayout = new QVBoxLayout();

    QPushButton *noteBtn = new QPushButton(title);
    noteBtn->setStyleSheet("text-align:left; padding:5px;");
    noteBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *deleteBtn = new QPushButton("×");
    deleteBtn->setStyleSheet("color:red; font-weight:bold;");
    deleteBtn->setFixedSize(25, 25);

    noteLayout->addWidget(noteBtn);
    noteLayout->addWidget(deleteBtn);

    m_notesLayout->addLayout(noteLayout);

    connect(noteBtn, &QPushButton::clicked, [this, title]() {
        if (!m_currentNoteTitle.isEmpty() && m_currentNoteTitle != title) {
            saveCurrentNote(); // Сохраняем предыдущую заметку перед переключением
        }
        selectNote(title);
    });

    connect(deleteBtn, &QPushButton::clicked, [this, title, noteLayout]() {
        deleteNote(title, noteLayout);
    });
}

void MainWindow::selectNote(const QString &title)
{
    m_isNoteLoading = true; // Устанавливаем флаг загрузки

    QSqlQuery query;
    query.prepare("SELECT content FROM notes WHERE title = ?");
    query.addBindValue(title);

    if (query.exec() && query.next()) {
        ui->titleEdit->setText(title);
        ui->contentEdit->setPlainText(query.value(0).toString());
        m_currentNoteTitle = title;
    }

    m_isNoteLoading = false; // Сбрасываем флаг загрузки
}

void MainWindow::onTextChanged()
{
    if (m_isNoteLoading || m_currentNoteTitle.isEmpty()) return;
    m_saveTimer->start(1000); // Запускаем таймер автосохранения
}

void MainWindow::saveCurrentNote()
{
    if (m_currentNoteTitle.isEmpty()) return;

    QString newTitle = ui->titleEdit->text().trimmed();
    QString newContent = ui->contentEdit->toPlainText();

    if (newTitle.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заголовок не может быть пустым");
        return;
    }

    QSqlQuery query;
    query.prepare("UPDATE notes SET title = ?, content = ? WHERE title = ?");
    query.addBindValue(newTitle);
    query.addBindValue(newContent);
    query.addBindValue(m_currentNoteTitle);

    if (!query.exec()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить заметку");
        return;
    }

    // Обновляем заголовок в интерфейсе, если он изменился
    if (m_currentNoteTitle != newTitle) {
        // Находим и обновляем кнопку с заметкой
        QWidget *container = ui->scrollArea->widget();
        if (container) {
            for (int i = 0; i < m_notesLayout->count(); ++i) {
                QLayoutItem *item = m_notesLayout->itemAt(i);
                if (item && item->layout()) {
                    QPushButton *btn = qobject_cast<QPushButton*>(item->layout()->itemAt(0)->widget());
                    if (btn && btn->text() == m_currentNoteTitle) {
                        btn->setText(newTitle);
                        break;
                    }
                }
            }
        }
        m_currentNoteTitle = newTitle;
    }

    qDebug() << "Заметка сохранена:" << newTitle;
}

void MainWindow::createNewNote()
{
    QString title = "Новая заметка " + QString::number(++m_noteCounter);
    QString content = "";

    QSqlQuery query;
    query.prepare("INSERT INTO notes (title, content) VALUES (?, ?)");
    query.addBindValue(title);
    query.addBindValue(content);

    if (!query.exec()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать заметку");
        return;
    }

    addNoteToUI(title, content);
    selectNote(title);
}

void MainWindow::deleteNote(const QString &title, QVBoxLayout *layout)
{
    QSqlQuery query;
    query.prepare("DELETE FROM notes WHERE title = ?");
    query.addBindValue(title);

    if (!query.exec()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить заметку");
        return;
    }

    // Удаляем из интерфейса
    QLayoutItem *item;
    while ((item = layout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
    delete layout;

    // Обновляем текущую заметку, если нужно
    if (m_currentNoteTitle == title) {
        m_currentNoteTitle.clear();
        ui->titleEdit->clear();
        ui->contentEdit->clear();
    }
}

MainWindow::~MainWindow()
{
    m_db.close();
    delete ui;
}
