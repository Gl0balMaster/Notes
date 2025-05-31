#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QInputDialog>
#pragma once


class TextNote : public QWidget {
    Q_OBJECT
public:
    explicit TextNote(const QString& text = "", QWidget* parent = nullptr)
        : QWidget(parent), m_text(text) {
        setupUI();
        setText(text);
    }

    void setText(const QString& text) {
        m_text = text;
        m_label->setText(text);
        adjustSize();
    }

    QString text() const { return m_text; }

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override {
        Q_UNUSED(event);
        startEditing();
    }

private:
    void setupUI() {
        m_layout = new QVBoxLayout(this);
        m_layout->setContentsMargins(5, 5, 5, 5);

        m_label = new QLabel(this);
        m_label->setWordWrap(true);
        m_label->setStyleSheet(
            "QLabel {"
            "  background: #FFF9C4;"
            "  padding: 8px;"
            "  border-radius: 4px;"
            "  border: 1px solid #FFE082;"
            "}"
            );

        m_layout->addWidget(m_label);
    }

    void startEditing() {
        m_editor = new QLineEdit(this);
        m_editor->setText(m_text);
        m_editor->setStyleSheet(
            "QLineEdit {"
            "  background: white;"
            "  padding: 8px;"
            "  border: 2px solid #64B5F6;"
            "  border-radius: 4px;"
            "}"
            );

        m_layout->replaceWidget(m_label, m_editor);
        m_label->hide();
        m_editor->setFocus();
        m_editor->selectAll();

        connect(m_editor, &QLineEdit::editingFinished, this, &TextNote::finishEditing);
        connect(m_editor, &QLineEdit::returnPressed, this, &TextNote::finishEditing);
    }

    void finishEditing() {
        if (m_editor) {
            setText(m_editor->text());
            m_layout->replaceWidget(m_editor, m_label);
            m_editor->deleteLater();
            m_editor = nullptr;
            m_label->show();
            emit textChanged(m_text);
        }
    }

signals:
    void textChanged(const QString& newText);

private:
    QString m_text;
    QLabel* m_label;
    QLineEdit* m_editor;
    QVBoxLayout* m_layout;
};
