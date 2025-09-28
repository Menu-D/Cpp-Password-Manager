#include <iostream> 
#include <string> 
#include <vector> 

class encrypter
{
public: 

	
};

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
		account acc; 
		acc.username = xor_encrypt_decrypt(user, key); 
		acc.password = xor_encrypt_decrypt(pass, key); 
		accounts.push_back({ user, pass }); 
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
	encrypter e1; 
	User u1; 
	std::string user, pass; 
	// Class objects 

	
	char key = 'm'; // XOR key 

	while (true)
	{
		std::cout << "please enter your username: ";
		std::cin >> user;

		std::cout << "please enter your password: ";
		std::cin >> pass;

		//stores encrypted user and pass 
		u1.addAccount(user, pass, key);

		std::cout << "\nStored accounts (decrypted): \n";
		u1.showAccounts(key);
	} 

	return 0; 
}