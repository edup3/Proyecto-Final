#pragma once

#include "constantes.hpp"

/**
 * Comprime los datos usando el algoritmo LZ77.
 * Formato de Token:
 * - Literal (2 bytes): [Flag 0x00] [Byte Literal]
 * - Coincidencia (4 bytes): [Flag 0x01] [Offset (2 bytes)] [Longitud (1 byte)]
 * @param input Datos binarios a comprimir.
 * @return Datos comprimidos.
 */
FileData compress_lz77(const FileData& input);

/**
 * Descomprime los datos comprimidos con LZ77.
 * @param input Datos binarios comprimidos.
 * @return Datos descomprimidos o vacío en caso de error.
 */
FileData decompress_lz77(const FileData& input);
