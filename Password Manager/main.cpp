#include <iostream> 
#include <string> 
#include <vector> 


class User
{
public: 
	struct account
	{
		std::string username;
		std::string password;
	};

	std::vector<account> accounts; 

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

}; 

int main()
{
	bool decode; 
	char answer; 

	// Class objects 
	User u1; 
	std::string user, pass; 
	// Class objects 

	
	char key;

		while (true)
		{
			int choice;

			std::cout << "Password Manager \n"; 
			std::cout << "\n";
			std::cout << "Enter new user/pass (1) " << '\n';
			std::cout << "\n";
			std::cout << "Check existing user/pass (2) " << '\n';
			std::cout << "\n";
			std::cout << "Exit (3) " << '\n'; 
			std::cout << "\n";

			std::cout << "Select one of the choices above: " << '\n';
			std::cin >> choice;

			if (choice == 1) 
			{
				std::cout << "Please enter your encryption key: "; 
				std::cin >> key; 
				std::cout << "\n"; 
				std::cout << "please enter your username: ";
				std::cin >> user;
				std::cout << "\n";
				std::cout << "please enter your password: ";
				std::cin >> pass;

				u1.addAccount(user, pass, key);
				std::cout << "\n";
				std::cout << "Your account was successfully added!\n"; 
			}
			else if (choice == 2) 
			{
				std::cout << "Please enter the decryption key: "; 
				std::cin >> key; 
				u1.showAccounts(key); 
			}
			else if (choice == 3) 
			{
				break; 
			}
			else {
				std::cout << "Invalid choice.\n"; 
			}

			
		}
	return 0; 
}