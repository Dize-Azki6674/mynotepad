#pragma once

#include <QtWidgets/QMainWindow>
#include <QStringView>
#include "ui_mynotepad.h"

struct FileType {
    QStringView dispname;
    QStringView suffix;
    QString filter() const;
};

namespace FileTypes {
    constexpr FileType any{
        .dispname{ u"すべてのファイル" },
        .suffix{ u"*" }
    };

    constexpr FileType text{
        .dispname{ u"テキストファイル" },
        .suffix{ u"txt" }
    };

    template <class T>
    concept FileTypeLike = requires(const T & ft) {
        { ft.filter() } -> std::same_as<QString>;
    };

    template <FileTypeLike... FTs>
    QString join_filters(const FTs&... filetypes)
    {
        QStringList filters;
        (filters.append(filetypes.filter()), ...);
        return filters.join(";;");
    };
}

class mynotepad : public QMainWindow
{
    Q_OBJECT

public:
    mynotepad(QWidget *parent = nullptr);
    ~mynotepad();

private:
    Ui::mynotepadClass ui;
    QString currentFile;

    void closeEvent(QCloseEvent* event) override;

    bool write_to(const QString& filename);
    [[maybe_unused]]
    bool save_file_with_dialog(const QString& caption);
    [[maybe_unused]]
    bool overwrite();

    bool close_manager();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionExit_triggered();
};