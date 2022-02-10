#include "mydialog.h"
#include "./ui_mydialog.h"

#include <QKeyEvent>

myDialog::myDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/_提示.png"));
    this->setWindowTitle("请注意提示信息");
    state = 0;
}

myDialog::~myDialog()
{
    delete ui;
}

void myDialog::setTitleTip(const QString &str)
{
    this->ui->label->setText(str);
}

void myDialog::setTip(const QString &str)
{
    this->ui->lineEdit->setText(str);
}

void myDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Return){
         on_bt_ok_clicked();
     }
}

bool myDialog::getCurState()
{
    return state;
}

QString myDialog::getLine1Text()
{
    return ui->l1->text();
}

QString myDialog::getLine2Text()
{
    return ui->l2->text();
}

void myDialog::setLine1Test(const QString& src)
{
    ui->l1->setPlaceholderText(src);
}

void myDialog::setLine2Test(const QString &src)
{
    ui->l2->setPlaceholderText(src);
}

QString myDialog::getLable1Text()
{
    return ui->t1->text();
}

QString myDialog::getLable2Text()
{
    return ui->t2->text();
}

void myDialog::setLable1Test(const QString &src)
{
    ui->t1->setText(src);
}

void myDialog::setLable2Test(const QString &src)
{
    ui->t2->setText(src);
}

void myDialog::hide1()
{
    ui->t1->setVisible(false);
    ui->l1->setVisible(false);
}

void myDialog::hide2()
{
    ui->t2->setVisible(false);
    ui->l2->setVisible(false);
}

void myDialog::hide_no()
{
    ui->bt_no->setVisible(false);
}

void myDialog::hide_ok()
{
    ui->bt_ok->setVisible(false);
}

void myDialog::on_bt_ok_clicked()
{
    this->accept();
    state = 1;
}


void myDialog::on_bt_no_clicked()
{
    this->reject();
    state = 0;
}

