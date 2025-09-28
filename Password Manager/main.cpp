#include <iostream> 
#include <string> 
#include <vector> 
#include <sodium.h>

class PasswordManager
{
public: 
	struct Account
	{
		std::string username;
		std::string password;
	};

	// XOR encryption / decryption function 
	std::string xor_encrypt_decrypt(const std::string& input, char key)
	{
		std::string output = input;
		for (size_t i = 0; i < input.size(); i++)
		{
			output[i] = input[i] ^ key;
		}
		return output;
	}

	void addAccount(const std::string& user, const std::string& pass, char key)
	{
		std::string encUser = xor_encrypt_decrypt(user, key); 
		std::string encPass = xor_encrypt_decrypt(pass, key); 
		accounts.push_back({ encUser, encPass }); 
	}

	void showAccounts(char key)
	{
		for (const auto& acc : accounts)
		{
			std::cout << "Username: " << xor_encrypt_decrypt(acc.username,key)
				<< ", Password: " << xor_encrypt_decrypt(acc.password,key)
				<< std::endl; 
		}
	}

private: 
	std::vector<Account> accounts;
}; 

int main()
{
	
	PasswordManager passwordmanager;
	std::string user, pass; 
	
	char key;

		while (false)
		{
			int choice;

			std::cout << "Password Manager \n"; 
			std::cout << "Enter new user/pass (1) " << std::endl;
			std::cout << "Check existing user/pass (2) " << std::endl;
			std::cout << "Exit (3) " << std::endl;

			std::cout << "Select one of the choices above: " << std::endl;
			std::cin >> choice;

			if (choice == 1) 
			{
				std::cout << "Please enter your encryption key: "; 
				std::cin >> key; 

				std::cout << "please enter your username: ";
				std::getline(std::cin, user); 

				std::cout << "\n";
				std::cout << "please enter your password: ";
				std::cin >> pass;

				passwordmanager.addAccount(user, pass, key);
				
				std::cout << "Your account was successfully added!\n"; 
			}
			else if (choice == 2) 
			{
				std::cout << "Please enter the decryption key: "; 
				std::cin >> key; 
				passwordmanager.showAccounts(key);
			}
			else if (choice == 3) 
			{
				break; 
			}
			else {
				std::cout << "Invalid choice." << std::endl;
			}


			if (sodium_init() < 0) return 1; 
			std::cout << "libsodium intialized successfully!\n"; 
			
		}
	return 0; 
}