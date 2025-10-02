#include "passwordmanager.h"
#include <vector>
#include <stdexcept>

PasswordManager::PasswordManager() {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium init failed!");
    }
}

std::string PasswordManager::encryptPassword(const std::string& password, const std::string& key) {
    if (key.size() != crypto_secretbox_KEYBYTES) {
        throw std::runtime_error("Key must be 32 bytes!");
    }

    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    std::vector<unsigned char> ciphertext(password.size() + crypto_secretbox_MACBYTES);
    crypto_secretbox_easy(ciphertext.data(),
                          reinterpret_cast<const unsigned char*>(password.data()),
                          password.size(),
                          nonce.data(),
                          reinterpret_cast<const unsigned char*>(key.data()));

    // Combine nonce + ciphertext
    std::string result(reinterpret_cast<char*>(nonce.data()), nonce.size());
    result.append(reinterpret_cast<char*>(ciphertext.data()), ciphertext.size());
    return result;
}

// This decrypts the ciphertext
std::string PasswordManager::decryptPassword(const std::string& cipher, const std::string& key) {
    if (key.size() != crypto_secretbox_KEYBYTES) {
        throw std::runtime_error("Key must be 32 bytes!");
    }

    if (cipher.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        throw std::runtime_error("Ciphertext too short!");
    }

    std::string nonce = cipher.substr(0, crypto_secretbox_NONCEBYTES);
    std::string ciphertext = cipher.substr(crypto_secretbox_NONCEBYTES);

    std::vector<unsigned char> decrypted(ciphertext.size() - crypto_secretbox_MACBYTES);

    if (crypto_secretbox_open_easy(decrypted.data(),
                                   reinterpret_cast<const unsigned char*>(ciphertext.data()),
                                   ciphertext.size(),
                                   reinterpret_cast<const unsigned char*>(nonce.data()),
                                   reinterpret_cast<const unsigned char*>(key.data())) != 0) {
        throw std::runtime_error("Decryption failed!");
    }

    return std::string(reinterpret_cast<char*>(decrypted.data()), decrypted.size());
}
