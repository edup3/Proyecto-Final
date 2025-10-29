#pragma once

#include <vector>
#include <string>

// Tipo para datos de archivo (byte sin signo)
using FileData = std::vector<unsigned char>;

// Definiciones de algoritmos soportados
const std::string COMP_ALG_LZ77 = "LZ77";
const std::string ENC_ALG_VIGENERE = "Vigenere";

// Constantes para LZ77
const int WINDOW_SIZE = 1024; // Tamaño máximo de la ventana de búsqueda
const int LOOKAHEAD_SIZE = 255; // Tamaño máximo de la coincidencia (limitado por 1 byte de longitud)

// Estructura para contener los parámetros de la operación
struct Config {
    bool compress = false;
    bool decompress = false;
    bool encrypt = false;
    bool decrypt = false;
    std::string input_path;
    std::string output_path;
    std::string key;
    std::string comp_alg = COMP_ALG_LZ77;
    std::string enc_alg = ENC_ALG_VIGENERE;
};
