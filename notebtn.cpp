#include "ExpandableNoteButton.h"

NoteBtn::NoteBtn(const QString& text, QWidget* parent)
    : QPushButton(text, parent), m_noteEditor(new QTextEdit(parent))
{
    setCheckable(true);
    setStyleSheet(R"(
        QPushButton {
            text-align: left;
            padding: 8px;
            border: 1px solid #ddd;
            background: #70c23a;
            color: black;
        }
        QPushButton:checked {
            background: #e0e0e0;
        }
    )");

    m_noteEditor->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #ddd;
            border-top: none;
            padding: 8px;
            background: white;
            color: black;
        }
    )");
    m_noteEditor->hide();

    connect(this, &QPushButton::toggled, [this](bool checked) {
        m_noteEditor->setVisible(checked);
        if (parentWidget()) {
            parentWidget()->adjustSize();
        }
    });
}

void NoteBtn::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);
    m_noteEditor->setFixedWidth(width());
}
