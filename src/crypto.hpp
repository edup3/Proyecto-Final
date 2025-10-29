#pragma once

#include "constantes.hpp"

/**
 * Encripta los datos usando el Cifrado Vigenère binario.
 * @param input Datos binarios a encriptar.
 * @param key Clave secreta.
 * @return Datos encriptados.
 */
FileData encrypt_vigenere(const FileData& input, const std::string& key);

/**
 * Desencripta los datos usando el Cifrado Vigenère binario.
 * @param input Datos binarios a desencriptar.
 * @param key Clave secreta.
 * @return Datos desencriptados.
 */
FileData decrypt_vigenere(const FileData& input, const std::string& key);
