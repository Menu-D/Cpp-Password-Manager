#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "passwordmanager.h"
#include <map>
#include <tuple>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
private:
    std::string sessionKey; // This stores the derived key for the session
    bool keySet = false; // Flag to make sure if the key has been set.

    std::map<std::pair<std::string, std::string>, std::string> passwordStore;
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSavePasswordClicked();
    void onGetPasswordClicked();

private:
    Ui::MainWindow *ui;
    PasswordManager pm;
};

#endif // MAINWINDOW_H
