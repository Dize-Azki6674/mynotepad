#include "stdafx.h"
#include "mynotepad.h"

#include <utility>

QString FileType::filter() const
{
    return suffix == "*"
        ? QString{ "%1 (*)" }.arg(dispname.toString())
        : QString{ "%1 (*.%2)" }.arg(dispname.toString(), suffix.toString());
}

mynotepad::mynotepad(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowTitle("新しいメモ");
}

mynotepad::~mynotepad()
{}

bool mynotepad::write_to(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "ファイルを保存できません: " + file.errorString());
        return false;
    }
    QTextStream out(&file);
    QString text = ui.textEdit->toPlainText();
    out << text;
    file.close();
    return true;
}

bool mynotepad::save_file_with_dialog(const QString& caption) {
    QString filename = QFileDialog::getSaveFileName(
        this,
        caption,
        "",
        FileTypes::join_filters(
            FileTypes::any,
            FileTypes::text
        )
    );
    if (filename.isEmpty())
        return false;

    bool write_res = write_to(filename);
    if (!write_res)
        return false;

    setWindowTitle(filename);
    currentFile = filename;
    ui.textEdit->document()->setModified(false);
    return true;
}

bool mynotepad::overwrite() {
    // ファイル名が無ければ上書き先のファイルを1つ選ぶ.
    if (currentFile.isEmpty()) {
        return save_file_with_dialog("保存");
    }
    else if (write_to(currentFile)) {
        setWindowTitle(currentFile);
        ui.textEdit->document()->setModified(false);
        return true;
    }

    return false;
}

bool mynotepad::close_manager() {
    if (!ui.textEdit->document()->isModified())
        return true;

    QMessageBox msgBox{ this };
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("あずきのメモ帳");
    msgBox.setText("変更が保存されていません．続行しますか？");
    msgBox.addButton(QMessageBox::Save)->setText("保存して閉じる");
    msgBox.addButton(QMessageBox::Discard)->setText("保存せずに閉じる");
    msgBox.addButton(QMessageBox::Cancel)->setText("キャンセル");
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Save:
        return overwrite();
    case QMessageBox::Discard:
        return true;
    case QMessageBox::Cancel:
        return false;
    default:
        // unreachable
        return false;
    }
}

void mynotepad::on_actionNew_triggered() {
    if (!close_manager())
        return;

    currentFile.clear();
    ui.textEdit->clear();
    ui.textEdit->document()->setModified(false);
    setWindowTitle("新しいメモ");
}

void mynotepad::on_actionOpen_triggered() {
    if (!close_manager())
        return;

    QString filename = QFileDialog::getOpenFileName(this, "ファイルを開く");
    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "警告", "ファイルを開けません: " + file.errorString());
        return;
    }

    currentFile = filename;
    setWindowTitle(filename);
    QTextStream in{ &file };
    QString text = in.readAll();
    ui.textEdit->setText(text);
    ui.textEdit->document()->setModified(false);

    file.close();
}

void mynotepad::on_actionSave_triggered() {
    overwrite();
}

void mynotepad::on_actionSaveAs_triggered() {
    save_file_with_dialog("名前を付けて保存");
}

void mynotepad::on_actionExit_triggered() {
    close();
}

void mynotepad::closeEvent(QCloseEvent* event) {
    if (close_manager())
        event->accept();
    else
        event->ignore();
}