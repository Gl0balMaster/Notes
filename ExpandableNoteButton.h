#pragma once
#include <QPushButton>
#include <QTextEdit>

class NoteBtn : public QPushButton {
    Q_OBJECT
public:
    explicit NoteBtn(const QString& text, QWidget* parent = nullptr);
    QTextEdit* noteEditor() const { return m_noteEditor; }

signals:
    void titleChanged(const QString& newTitle);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QTextEdit* m_noteEditor;
};
