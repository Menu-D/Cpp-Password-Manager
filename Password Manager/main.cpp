#include <sodium.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>

class PasswordManager 
{
private:
    struct Account 
    {
        // Salt used for deriving the key for this account (crypto_pwhash expects this size)
        unsigned char salt[crypto_pwhash_SALTBYTES];

        // Nonces for username and password encryption (unique per encryption)
        unsigned char nonce_user[crypto_secretbox_NONCEBYTES];
        unsigned char nonce_pass[crypto_secretbox_NONCEBYTES];

        // ciphertext for username and password
        std::vector<unsigned char> ciphertext_user; // message_len + MACBYTES
        std::vector<unsigned char> ciphertext_pass;
    };

    std::vector<Account> accounts;

    // KDF parameters - interactive defaults (tweak later if you want slower/harder)
    static constexpr unsigned long long OPSLIMIT = crypto_pwhash_OPSLIMIT_MODERATE;
    static constexpr size_t MEMLIMIT = crypto_pwhash_MEMLIMIT_MODERATE;

    // Helper: derive a key of crypto_secretbox_KEYBYTES from passphrase + salt
    bool derive_key_from_passphrase(const std::string& passphrase, const unsigned char salt[crypto_pwhash_SALTBYTES],
        unsigned char out_key[crypto_secretbox_KEYBYTES]) 
    {
        if (crypto_pwhash(out_key, crypto_secretbox_KEYBYTES,
            passphrase.c_str(), passphrase.size(),
            salt,
            OPSLIMIT, MEMLIMIT, crypto_pwhash_ALG_DEFAULT) != 0) 
        {
            return false; // out of memory or similar
        }
        return true;
    }

    // Helper: encrypt a string -> ciphertext vector (message + MAC). Caller supplies nonce and key.
    std::vector<unsigned char> encrypt_string(const std::string& plain,
        const unsigned char nonce[crypto_secretbox_NONCEBYTES],
        const unsigned char key[crypto_secretbox_KEYBYTES]) 
    {
        std::vector<unsigned char> cipher(plain.size() + crypto_secretbox_MACBYTES);
        crypto_secretbox_easy(cipher.data(),
            reinterpret_cast<const unsigned char*>(plain.data()), plain.size(),
            nonce, key);
        return cipher;
    }

    // Helper: decrypt ciphertext -> plaintext string. Returns empty + false on failure.
    bool decrypt_string(const std::vector<unsigned char>& cipher,
        const unsigned char nonce[crypto_secretbox_NONCEBYTES],
        const unsigned char key[crypto_secretbox_KEYBYTES],
        std::string& out_plain) 
    {
        if (cipher.size() < crypto_secretbox_MACBYTES) return false;
        std::vector<unsigned char> recovered(cipher.size() - crypto_secretbox_MACBYTES);
        if (crypto_secretbox_open_easy(recovered.data(),
            cipher.data(), cipher.size(),
            nonce, key) != 0) {
            return false; // verification failed (wrong key or tampered)
        }
        out_plain.assign(reinterpret_cast<char*>(recovered.data()), recovered.size());
        return true;
    }

public:
    PasswordManager() = default;

    // Add an account encrypted with a key derived from passphrase
    void addAccount(const std::string& username, const std::string& password, const std::string& masterPassphrase) 
    {
        Account a;

        // 1) generate random salt and nonces
        randombytes_buf(a.salt, sizeof a.salt);
        randombytes_buf(a.nonce_user, sizeof a.nonce_user);
        randombytes_buf(a.nonce_pass, sizeof a.nonce_pass);

        // 2) derive key for this account from the passphrase + salt (Argon2)
        unsigned char key[crypto_secretbox_KEYBYTES];
        if (!derive_key_from_passphrase(masterPassphrase, a.salt, key)) 
        {
            std::cerr << "Key derivation failed (out of memory?). Account not added.\n";
            return;
        }

        // 3) encrypt username and password with the derived key and nonces
        a.ciphertext_user = encrypt_string(username, a.nonce_user, key);
        a.ciphertext_pass = encrypt_string(password, a.nonce_pass, key);

        // 4) wipe the key from memory
        sodium_memzero(key, sizeof key);

        accounts.push_back(std::move(a));
        std::cout << "Account encrypted and stored in memory.\n";
    }

    // Show all accounts (attempt decryption using provided passphrase)
    void showAccounts(const std::string& masterPassphrase) 
    {
        if (accounts.empty()) 
        {
            std::cout << "No accounts stored.\n";
            return;
        }

        bool any = false;
        for (size_t i = 0; i < accounts.size(); ++i) 
        {
            const Account& a = accounts[i];

            unsigned char key[crypto_secretbox_KEYBYTES];
            if (!derive_key_from_passphrase(masterPassphrase, a.salt, key)) 
            {
                std::cerr << "[!] Key derivation failed while reading accounts.\n";
                continue;
            }

            std::string user, pass;
            bool ok_user = decrypt_string(a.ciphertext_user, a.nonce_user, key, user);
            bool ok_pass = decrypt_string(a.ciphertext_pass, a.nonce_pass, key, pass);

            // wipe key immediately
            sodium_memzero(key, sizeof key);

            std::cout << "----- Account #" << i + 1 << " -----\n";
            if (ok_user && ok_pass) 
            {
                std::cout << "Username: " << user << "\n";
                std::cout << "Password: " << pass << "\n";
            }
            else 
            {
                std::cout << "Decryption failed: wrong passphrase or data corrupted.\n";
            }
            any = true;
        }

        if (!any) std::cout << "No readable accounts found (maybe wrong passphrase).\n";
    }

    // Convenience: list number of accounts (no plaintext)
    size_t count() const { return accounts.size(); }
};

static void clear_stdin_line() 
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() 
{
    if (sodium_init() < 0) 
    {
        std::cerr << "libsodium initialization failed\n";
        return 1;
    }

    PasswordManager pm;

    while (true) 
    {
        std::cout << "\nPassword Manager (*memory only*)\n";
        std::cout << "1) Add new account\n";
        std::cout << "2) Show accounts (requires passphrase)\n";
        std::cout << "3) Count accounts\n";
        std::cout << "4) Exit\n";
        std::cout << "Choose: ";
        int choice;
        if (!(std::cin >> choice)) 
        {
            std::cin.clear();
            clear_stdin_line();
            std::cout << "Invalid input.\n";
            continue;
        }
        clear_stdin_line();

        if (choice == 1) 
        {
            std::string passphrase;
            std::cout << "Master passphrase (used to derive key for this account): ";
            std::getline(std::cin, passphrase);

            std::string username, password;
            std::cout << "Username: ";
            std::getline(std::cin, username);
            std::cout << "Password: ";
            std::getline(std::cin, password);

            pm.addAccount(username, password, passphrase);
        }
        else if (choice == 2) 
        {
            std::string passphrase;
            std::cout << "Master passphrase to try for decryption: ";
            std::getline(std::cin, passphrase);
            pm.showAccounts(passphrase);
        }
        else if (choice == 3) 
        {
            std::cout << "Accounts stored (encrypted blobs): " << pm.count() << std::endl;
        }
        else if (choice == 4) 
        {
            break;
        }
        else 
        {
            std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
