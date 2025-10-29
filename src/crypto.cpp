#include "crypto.hpp"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Convierte la clave a un vector de bytes sin signo
static vector<unsigned char> prepare_key(const string& key) {
    vector<unsigned char> prepared_key;
    for (char c : key) {
        prepared_key.push_back(static_cast<unsigned char>(c));
    }
    return prepared_key;
}

FileData encrypt_vigenere(const FileData& input, const string& key) {
    if (input.empty() || key.empty()) return input;

    FileData output = input;
    vector<unsigned char> prepared_key = prepare_key(key);
    size_t key_len = prepared_key.size();

    for (size_t i = 0; i < input.size(); ++i) {
        unsigned char plain_byte = input[i];
        unsigned char key_byte = prepared_key[i % key_len];

        // Encriptación binaria: C = (P + K) mod 256
        output[i] = static_cast<unsigned char>(plain_byte + key_byte);
    }
    return output;
}

FileData decrypt_vigenere(const FileData& input, const string& key) {
    if (input.empty() || key.empty()) return input;

    FileData output = input;
    vector<unsigned char> prepared_key = prepare_key(key);
    size_t key_len = prepared_key.size();

    for (size_t i = 0; i < input.size(); ++i) {
        unsigned char cipher_byte = input[i];
        unsigned char key_byte = prepared_key[i % key_len];

        // Desencriptación binaria: P = (C - K) mod 256
        output[i] = static_cast<unsigned char>(cipher_byte - key_byte);
    }
    return output;
}