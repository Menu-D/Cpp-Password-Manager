#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sodium.h>
#include <vector>
#include <stdexcept>


// This is the utility function which will derive a key from the master password
std::string deriveKey(const QString& masterPassword)
{
    std::vector<unsigned char> key(crypto_secretbox_KEYBYTES);

    // This uses a fixed salt for now as we can later store and retrieve from the DB itself

    const unsigned char salt[crypto_pwhash_SALTBYTES] = {0};

    if(crypto_pwhash(
            key.data(),
            key.size(),
            masterPassword.toStdString().c_str(),
            masterPassword.size(),
            salt,
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE,
            crypto_pwhash_ALG_DEFAULT) !=0)
    {
        throw std::runtime_error("Key derivation failed!");
    }

    return std::string(reinterpret_cast<char*>(key.data()), key.size());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect buttons to slots
    connect(ui->SavePasswordButton, &QPushButton::clicked,
            this, &MainWindow::onSavePasswordClicked);

    connect(ui->GetPasswordButton, &QPushButton::clicked,
            this, &MainWindow::onGetPasswordClicked);

    connect(ui->SetMasterPasswordButton, &QPushButton::clicked,
            this, [this]() {
                QString master = ui->MasterPassword->text();
                if(master.isEmpty()) {
                    ui->StatusLabel->setText("Enter a master password first!");
                    return;
                }

                try {
                    sessionKey = deriveKey(master); // This uses the same deriveKey function
                    keySet = true;
                    ui->MasterPassword->clear();    // This clears the input
                    ui->StatusLabel->setText("Master password set for this session.");
                } catch (const std::exception &e) {
                    ui->StatusLabel->setText("Error: " + QString::fromStdString(e.what()));
                }
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSavePasswordClicked()
{
    if(!keySet) {
        ui->StatusLabel->setText("Set a master password first!");
        return;
    }

    QString service = ui->Service->text();
    QString username = ui->Username->text();
    QString password = ui->Password->text();

    try {
        std::string encrypted = pm.encryptPassword(password.toStdString(), sessionKey);
        passwordStore[{service.toStdString(), username.toStdString()}] = encrypted;

        ui->StatusLabel->setText("Password saved for " + service);
        ui->Password->clear();
    } catch (const std::exception &e) {
        ui->StatusLabel->setText("Error: " + QString::fromStdString(e.what()));
    }
}

void MainWindow::onGetPasswordClicked()
{
    if(!keySet) {
        ui->StatusLabel->setText("Set a master password first!");
        return;
    }

    QString service = ui->Service->text();
    QString username = ui->Username->text();

    auto it = passwordStore.find({service.toStdString(), username.toStdString()});
    if(it == passwordStore.end()) {
        ui->StatusLabel->setText("Error: No password saved for this service/user");
        return;
    }

    try {
        std::string decrypted = pm.decryptPassword(it->second, sessionKey);
        ui->StatusLabel->setText("Password: " + QString::fromStdString(decrypted));
    } catch(const std::exception &e) {
        ui->StatusLabel->setText("Error: " + QString::fromStdString(e.what()));
    }
}
