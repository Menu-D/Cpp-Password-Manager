#include "cui.h"
#include "log.h"
#include <iostream> 
#include <string>
#include <vector> 

Vault VH;

    void RunProgram::runProgram()
    {
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
                VH.clear_stdin_line();
                std::cout << "Invalid input.\n";
                continue;
            }
            VH.clear_stdin_line();

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

                VH.addAccount(username, password, passphrase);
            }
            else if (choice == 2)
            {
                std::string passphrase;
                std::cout << "Master passphrase to try for decryption: ";
                std::getline(std::cin, passphrase);
                VH.showAccounts(passphrase);
            }
            else if (choice == 3)
            {
                std::cout << "Accounts stored (encrypted blobs): " << VH.count() << std::endl;
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
    }

    
