#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>
#include <sodium.h>

class PasswordManager {
public:
    PasswordManager();

    std::string encryptPassword(const std::string& password, const std::string& key);
    std::string decryptPassword(const std::string& cipher, const std::string& key);
};

#endif // PASSWORDMANAGER_H
