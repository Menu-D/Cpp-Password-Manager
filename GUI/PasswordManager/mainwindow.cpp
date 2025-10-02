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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSavePasswordClicked()
{
    QString service = ui->Service->text();
    QString username = ui->Username->text();
    QString password = ui->Password->text();
    QString master = ui -> MasterPassword -> text();

    // Encrypt + save (for now, just memory)
    try {
        std::string key(crypto_secretbox_KEYBYTES, 'K'); // temporary key
        std::string encrypted = pm.encryptPassword(password.toStdString(), key);

        // Temporary storage in Map
        passwordStore[{service.toStdString(), username.toStdString()}] = encrypted;
        // store somewhere (in-memory map, db later)
        // For now: just show success
        ui->StatusLabel->setText("Password saved for " + service);

        ui->Password-> clear(); //<- This clears the password field

    } catch (const std::exception &e) {
        ui->StatusLabel->setText("Error: " + QString::fromStdString(e.what()));
    }
}

void MainWindow::onGetPasswordClicked()
{
    QString service = ui->Service->text();
    QString username = ui->Username->text();
    QString master = ui-> MasterPassword -> text();

    auto it = passwordStore.find({service.toStdString(), username.toStdString()});
    if(it == passwordStore.end())
    {
        ui->StatusLabel->setText("Error: No password was saved for this user/service");
        return;
    }

    try {
        std::string key(crypto_secretbox_KEYBYTES, 'K'); // same key
        std::string decrypted = pm.decryptPassword(it->second, key);

        ui->StatusLabel->setText("Password: " + QString::fromStdString(decrypted));
    } catch (const std::exception &e) {
        ui->StatusLabel->setText("Error: " + QString::fromStdString(e.what()));
    }
}
