#pragma once

#include "constantes.hpp"
#include <string>
#include <vector>

/**
 * Lee el contenido completo de un archivo usando llamadas de sistema POSIX.
 * @param filename Ruta del archivo.
 * @return Datos leídos, o vector vacío en caso de error.
 */
FileData read_file_posix(const std::string& filename);

/**
 * Escribe datos en un archivo usando llamadas de sistema POSIX.
 * @param filename Ruta del archivo de salida.
 * @param data Datos binarios a escribir.
 * @return true si la escritura fue exitosa, false en caso contrario.
 */
bool write_file_posix(const std::string& filename, const FileData& data);

/**
 * Determina si la ruta es un directorio.
 * @param path Ruta a verificar.
 * @return true si es un directorio, false en caso contrario.
 */
bool is_directory(const std::string& path);

/**
 * Lista los archivos regulares en un directorio.
 * @param path Ruta del directorio.
 * @return Vector de rutas absolutas de archivos regulares.
 */
std::vector<std::string> list_directory(const std::string& path);