#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>

namespace Ui {
class myDialog;
}

class myDialog : public QDialog
{
    Q_OBJECT
    bool state;
public:
    explicit myDialog(QWidget *parent = nullptr);
    ~myDialog();
    void setTitleTip(const QString& str);
    void setTip(const QString& str);

    bool getCurState();
    QString getLine1Text();
    QString getLine2Text();

    void setLine1Test(const QString& src);
    void setLine2Test(const QString& src);

    QString getLable1Text();
    QString getLable2Text();
    void setLable1Test(const QString& src);
    void setLable2Test(const QString& src);

    void hide1();
    void hide2();
    void hide_no();
    void hide_ok();
private slots:
    void on_bt_ok_clicked();

    void on_bt_no_clicked();

private:
    Ui::myDialog *ui;
};

#endif // MYDIALOG_H
