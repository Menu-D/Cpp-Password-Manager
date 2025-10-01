#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "passwordmanager.h"

PasswordManager pm("test_master_key");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::savePassword);
    connect(ui->getButton, &QPushButton::clicked, this, &MainWindow::getPassword);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::savePassword()
{
    QString service = ui->serviceEdit->text();
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (pm.storePassword(service.toStdString(),
                         username.toStdString(),
                         password.toStdString())) {
        ui->statusLabel->setText("Password saved!");
    } else {
        ui->statusLabel->setText("Save failed!");
    }
}

void MainWindow::getPassword()
{
    QString service = ui->serviceEdit->text();
    QString username = ui->usernameEdit->text();

    std::string result = pm.retrievePassword(service.toStdString(),
                                             username.toStdString());

    if (!result.empty()) {
        ui->statusLabel->setText(QString::fromStdString(result));
    } else {
        ui->statusLabel->setText("Not found!");
    }
}
