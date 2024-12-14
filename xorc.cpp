// Simple xor encript for small files
// Criptografia simples em xor para arquivos pequenos 

#include <iostream>
#include <string>
#include "sha256.h"

// 0 🪟
// 1 🐧
int OS = 0;

#if OS == 0
	#include <windows.h>
#endif

// Global variables
// TODO: change to user input in terminal
std::string _PASS = "";

unsigned char _KEY[1024];
unsigned int file_size = 0;

// Generete a random 1024 key size using a password
void pass_to_key() {
	// Get the hash of it
	std::string pass_hash = picosha2::hash256_hex_string(_PASS);
	size_t hash_size = pass_hash.length();

	// Key alphabet
	std::string alpha = "qwfpbjluyarstgmneioxcdvzkhQWFPBJLUYARSTGMNEIOXCDVZKH1234567890<>[]{}!@#$%&*()_+;-";
	size_t alpha_size = alpha.length();

	// Final key size
	int key_size = 1024;

	// Init the rand seed
	srand(_PASS.length() * 541348);

	// Generate the random key
	for (size_t i = 0; i < key_size - 1; i++)
	{
		int rd = rand() % 81 + 1;
		_KEY[i] = alpha[(i + rd) % alpha_size] ^ pass_hash[i % hash_size];
	}
}

char* crypt(std::string path) {
	// Read the original file
	std::ifstream myfile(path, std::ios::in|std::ios::binary);
	myfile.seekg(0, std::ios::end);   
	file_size = myfile.tellg();
	myfile.seekg(0, std::ios::beg);

	// File bytes are stored in the buffer vriable
	char* buffer = new char[file_size];
	myfile.read(buffer,file_size);
	myfile.close();

	// Derivation rounds
	for (size_t i = 1; i <= 10; i++) {
		// temp key
		unsigned char der_key[1024];
		for (size_t o = 0; o < 1024; o++)
		{
			//  Make a simple derivation
			der_key[o] = (_KEY[o] + i);
		}

		// split the key in 4
		for (size_t e = 0; e < 4; e++)
		{
			// Key offset
			int offset = (256 * e);

			// Run all the file
			for (size_t k = 0; k < file_size; k++)
			{
				// encrypt using the derived key
				buffer[k] ^= der_key[offset + (k % 256)];
			}
		}
	}

	return buffer;
}

void write_file(std::string path, char* bytes) {
	std::ofstream myfile(path, std::ios::out|std::ios::binary);
	myfile.write(bytes, file_size);
	myfile.close();
}


void banner() {
	std::cout << "Warning!\nThe input file will be overwriten, the file can be lost" << std::endl;
}


int main(int argc, char *argv[]) {
	banner();
	// Get the pass from input
	#if OS == 0
		std::cout << "Pass: ";

		// Disable text input on console
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode;
		GetConsoleMode(hStdin, &mode);
		SetConsoleMode(hStdin, mode & ~ENABLE_ECHO_INPUT);

		std::getline(std::cin, _PASS);

		// Reset
		SetConsoleMode(hStdin, mode);
	#endif

	// Generate the key
	pass_to_key();

	// Get the file path from the first argv in console
	std::string file_path = argv[1];

	// Encrypt using the key
	char* enc_data = crypt(file_path);

	// Overwrite the original file
	write_file(file_path, enc_data);

	return 0;
}