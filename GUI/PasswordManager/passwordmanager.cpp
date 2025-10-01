#include "passwordmanager.h"
#include <unordered_map>

static std::unordered_map<std::string, std::string> dummyStore;

PasswordManager::PasswordManager(const std::string& key) : masterKey(key) {}

bool PasswordManager::storePassword(const std::string& service, const std::string& username, const std::string& password) {
    std::string key = service + ":" + username;
    dummyStore[key] = password;
    return true;
}

std::string PasswordManager::retrievePassword(const std::string& service, const std::string& username) {
    std::string key = service + ":" + username;
    if (dummyStore.find(key) != dummyStore.end()) {
        return dummyStore[key];
    }
    return "";
}
