#include "compress.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

FileData compress_lz77(const FileData& input) {
    if (input.empty()) return {};

    FileData output;
    size_t pos = 0;

    while (pos < input.size()) {
        size_t best_match_len = 0;
        size_t best_match_offset = 0;

        // Definir la ventana de búsqueda y el buffer de búsqueda
        size_t start_window = (pos < WINDOW_SIZE) ? 0 : pos - WINDOW_SIZE;
        size_t lookahead_limit = min(input.size() - pos, (size_t)LOOKAHEAD_SIZE);

        // Buscar la coincidencia más larga en la ventana
        for (size_t i = start_window; i < pos; ++i) {
            size_t current_match_len = 0;
            size_t current_offset = pos - i;

            // Comparar hasta el límite del buffer
            while (current_match_len < lookahead_limit && input[i + current_match_len] == input[pos + current_match_len]) {
                current_match_len++;
            }

            // Actualizar la mejor coincidencia
            if (current_match_len > best_match_len) {
                best_match_len = current_match_len;
                best_match_offset = current_offset;
            }
        }

        // Si se encuentra una coincidencia significativa (longitud > 1)
        if (best_match_len > 1) {
            // MATCH Token (4 bytes): [Flag 0x01] [Offset MSB] [Offset LSB] [Length]
            output.push_back(0x01); // Flag de coincidencia

            // Offset de 2 bytes (big-endian)
            output.push_back(static_cast<unsigned char>((best_match_offset >> 8) & 0xFF));
            output.push_back(static_cast<unsigned char>(best_match_offset & 0xFF));
            
            // Longitud de 1 byte
            output.push_back(static_cast<unsigned char>(best_match_len));

            pos += best_match_len; // Avanzar la posición por la longitud de la coincidencia
        } else {
            // LITERAL Token (2 bytes): [Flag 0x00] [Byte Literal]
            output.push_back(0x00); // Flag literal
            output.push_back(input[pos]); // Byte literal

            pos++; // Avanzar la posición por 1 (el byte literal)
        }
    }
    return output;
}

FileData decompress_lz77(const FileData& input) {
    if (input.empty()) return {};

    FileData output;
    size_t input_pos = 0;

    try {
        while (input_pos < input.size()) {
            unsigned char flag = input[input_pos++];
            
            if (flag == 0x00) {
                // LITERAL: [Flag 0x00] [Byte Literal]
                if (input_pos >= input.size()) throw runtime_error("Estructura de literal incompleta.");
                output.push_back(input[input_pos++]);

            } else if (flag == 0x01) {
                // MATCH: [Flag 0x01] [Offset MSB] [Offset LSB] [Length]
                if (input_pos + 2 >= input.size()) throw runtime_error("Estructura de match incompleta (Offset).");
                
                // Leer Offset (2 bytes)
                size_t offset = (input[input_pos] << 8) | input[input_pos + 1];
                input_pos += 2;
                
                if (input_pos >= input.size()) throw runtime_error("Estructura de match incompleta (Length).");
                unsigned char length = input[input_pos++];

                if (offset == 0 || length == 0) throw runtime_error("Match con Offset o Longitud cero.");

                // Copiar bytes del buffer de salida (manejo de superposición)
                size_t start_copy = output.size() - offset;

                if (start_copy >= output.size()) throw runtime_error("Offset fuera de límites.");

                for (size_t i = 0; i < length; ++i) {
                    // Copia secuencial para manejar la superposición
                    output.push_back(output[start_copy + i]);
                }
            } else {
                throw runtime_error("Bandera de token desconocida.");
            }
        }
    } catch (const runtime_error& e) {
        cerr << "ERROR LZ77 DECOMPRESSION: Falló en la posición " << input_pos << ": " << e.what() << endl;
        return {};
    }

    return output;
}