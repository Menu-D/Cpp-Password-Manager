#pragma once 
#include <vector>
#include <sodium.h>
#include <string>
#include "database.h"

class CryptoHandler
{
public:
    struct Account {
        std::vector<unsigned char> ciphertext_user;
        unsigned char nonce_user[crypto_secretbox_NONCEBYTES];

        std::vector<unsigned char> ciphertext_pass;
        unsigned char nonce_pass[crypto_secretbox_NONCEBYTES];

        unsigned char salt[crypto_pwhash_SALTBYTES];
    };

    // Functions...
    bool derive_key_from_passphrase(const std::string& passphrase, const unsigned char salt[crypto_pwhash_SALTBYTES],
        unsigned char out_key[crypto_secretbox_KEYBYTES]);

    std::vector<unsigned char> encrypt_string(const std::string& plain,
        const unsigned char nonce[crypto_secretbox_NONCEBYTES],
        const unsigned char key[crypto_secretbox_KEYBYTES]);

    bool decrypt_string(const std::vector<unsigned char>& cipher,
        const unsigned char nonce[crypto_secretbox_NONCEBYTES],
        const unsigned char key[crypto_secretbox_KEYBYTES],
        std::string& out_plain);

    CryptoHandler() = default;

    std::vector<Account> accounts; // <- store accounts here
};


class Vault
{
    CryptoHandler CH; // Trying to give vault its own cryptohandler instance. 
    Database* db; // This is an optional link to DB. 

public:

    Vault(Database* database = nullptr) : db(database) {}

    void addAccount(const std::string& username, const std::string& password, const std::string& masterPassphrase);

    void showAccounts(const std::string& masterPassphrase);

    static void clear_stdin_line();

    size_t count() const { return CH.accounts.size(); }
};