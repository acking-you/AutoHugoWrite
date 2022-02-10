#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QProcess>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<QString> GetProcessIDByPort(uint port);
private:
    void startCmd(const QString&);
     void setBtnFromState(bool);
private slots:
    void onStandardOutput();
    void onErrorOutput();
    void on_choose_pos_clicked();

    void on_show_categories_clicked();
    void on_start_write_clicked();

    void on_show_Blog_clicked();

    void on_debug_clicked();

    void on_addImg_clicked();



    void on_open_categories_clicked();

    void on_open_imgfile_clicked();

    void on_init_clicked();

    void on_release_clicked();

    void on_check_processByport_clicked();

    void on_kill_porcessbyPID_clicked();

    void on_setEditor_clicked();

private:
    void show_Blogpath();

private:
    Ui::MainWindow *ui;
    QProcess* m_Process;
    QString BlogPath;
    QString githuboRepoPath;
    QString giteeRepoPath;
    bool isInput;
};
#endif // MAINWINDOW_H
