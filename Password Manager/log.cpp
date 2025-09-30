#include "log.h"
#include <iostream>
#include <sodium.h>

// CryptoHandler methods
bool CryptoHandler::derive_key_from_passphrase(const std::string& passphrase,
    const unsigned char salt[crypto_pwhash_SALTBYTES],
    unsigned char out_key[crypto_secretbox_KEYBYTES])
{
    return crypto_pwhash(
        out_key, crypto_secretbox_KEYBYTES,
        passphrase.c_str(), passphrase.size(),
        salt,
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE,
        crypto_pwhash_ALG_DEFAULT) == 0;
}

std::vector<unsigned char> CryptoHandler::encrypt_string(const std::string& plain,
    const unsigned char nonce[crypto_secretbox_NONCEBYTES],
    const unsigned char key[crypto_secretbox_KEYBYTES])
{
    std::vector<unsigned char> cipher(plain.size() + crypto_secretbox_MACBYTES);
    crypto_secretbox_easy(cipher.data(),
        reinterpret_cast<const unsigned char*>(plain.data()), plain.size(),
        nonce, key);
    return cipher;
}

bool CryptoHandler::decrypt_string(const std::vector<unsigned char>& cipher,
    const unsigned char nonce[crypto_secretbox_NONCEBYTES],
    const unsigned char key[crypto_secretbox_KEYBYTES],
    std::string& out_plain)
{
    std::vector<unsigned char> plain(cipher.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(
        plain.data(), cipher.data(), cipher.size(),
        nonce, key) != 0)
    {
        return false; // Decryption failed
    }
    out_plain.assign(reinterpret_cast<char*>(plain.data()), plain.size());
    return true;
}

// Vault methods
void Vault::addAccount(const std::string& username, const std::string& password, const std::string& masterPassphrase)
{
    CryptoHandler::Account acc;

    // This generates a random salt + nonces
    randombytes_buf(acc.salt, sizeof acc.salt);
    randombytes_buf(acc.nonce_user, sizeof acc.nonce_user);
    randombytes_buf(acc.nonce_pass, sizeof acc.nonce_pass);

    // Derive key
    unsigned char key[crypto_secretbox_KEYBYTES];
    if (!CH.derive_key_from_passphrase(masterPassphrase, acc.salt, key))
    {
        std::cerr << "[!] Key derivation failed\n";
        return;
    }

    // This encrypts the username and password
    acc.ciphertext_user = CH.encrypt_string(username, acc.nonce_user, key);
    acc.ciphertext_pass = CH.encrypt_string(password, acc.nonce_pass, key);

    // Store account
    CH.accounts.push_back(acc);

    if (db)
    {
        if (!db->insertAccount(acc))
        {
            std::cerr << "[!] Failed to save account to database! \n"; 
        }
    }

    // This is the wipe key
    sodium_memzero(key, sizeof key);
}

void Vault::showAccounts(const std::string& masterPassphrase)
{

    std::vector<CryptoHandler::Account> sourceAccounts;

    if (db)
    {
        db->fetchAccounts(sourceAccounts);
    }
    else
    {
        sourceAccounts = CH.accounts; 
    }

    if (sourceAccounts.empty())
    {
        std::cout << "No accounts stored.\n";
        return;
    }

    for (size_t i = 0; i < sourceAccounts.size(); ++i)
    {
        const auto& a = sourceAccounts[i]; 

        unsigned char key[crypto_secretbox_KEYBYTES];
        if (!CH.derive_key_from_passphrase(masterPassphrase, a.salt, key))
        {
            std::cerr << "[!] Key derivation failed while reading accounts.\n";
            continue;
        }

        std::string user, pass;
        bool ok_user = CH.decrypt_string(a.ciphertext_user, a.nonce_user, key, user);
        bool ok_pass = CH.decrypt_string(a.ciphertext_pass, a.nonce_pass, key, pass);

        sodium_memzero(key, sizeof key);

        std::cout << "----- Account #" << i + 1 << " -----\n";
        if (ok_user && ok_pass)
        {
            std::cout << "Username: " << user << "\n";
            std::cout << "Password: " << pass << "\n";
        }
        else
        {
            std::cout << "Decryption failed: wrong passphrase or corrupted data.\n";
        }
    }
}

void Vault::clear_stdin_line()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
