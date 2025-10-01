#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>

class PasswordManager {
public:
    PasswordManager(const std::string& key);
    bool storePassword(const std::string& service, const std::string& username, const std::string& password);
    std::string retrievePassword(const std::string& service, const std::string& username);

private:
    std::string masterKey;
};

#endif // PASSWORDMANAGER_H
