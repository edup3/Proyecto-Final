#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h> 
#include <errno.h>    


// Inclusión de módulos
#include "constantes.hpp"
#include "crypto.hpp"
#include "compress.hpp"
#include "fs_utils.hpp"

using namespace std;

// =================================================================
// LÓGICA DE PROCESAMIENTO
// =================================================================

/**
 * Función worker para procesar un solo archivo.
 * Llama a las funciones de compresión/encriptación en el orden correcto.
 */
void process_file(const string& input_file, const string& output_file, const Config& config) {
    cout << "  [HILO] Procesando: " << input_file << " -> " << output_file << endl;

    FileData data = read_file_posix(input_file);
    if (data.empty()) {
        cerr << "  [HILO] ERROR: Falló la lectura o el archivo está vacío: " << input_file << endl;
        return;
    }

    FileData processed_data = data;

    // 1. Desencriptar (si -u)
    if (config.decrypt) {
        processed_data = decrypt_vigenere(processed_data, config.key);
        cout << "  [HILO] Desencriptado (Vigenere): " << input_file << endl;
    }

    // 2. Descomprimir (si -d)
    if (config.decompress) {
        processed_data = decompress_lz77(processed_data);
        if (processed_data.empty()) {
            cerr << "  [HILO] ERROR: Falló la descompresión LZ77 de: " << input_file << endl;
            return;
        }
        cout << "  [HILO] Descomprimido (LZ77): " << input_file << endl;
    }

    // 3. Comprimir (si -c)
    if (config.compress) {
        processed_data = compress_lz77(processed_data);
        cout << "  [HILO] Comprimido (LZ77): " << input_file << endl;
    }

    // 4. Encriptar (si -e)
    if (config.encrypt) {
        processed_data = encrypt_vigenere(processed_data, config.key);
        cout << "  [HILO] Encriptado (Vigenere): " << input_file << endl;
    }

    // Escribir el resultado
    if (write_file_posix(output_file, processed_data)) {
        cout << "  [HILO] Éxito. Resultado guardado en: " << output_file << endl;
    } else {
        cerr << "  [HILO] ERROR: Falló la escritura en el archivo de salida: " << output_file << endl;
    }
}

/**
 * Procesa todos los archivos en un directorio usando hilos (Concurrencia).
 */
void process_directory(const Config& config) {
    cout << "--- Modo Directorio: Iniciando procesamiento concurrente ---" << endl;
    
    // Crear el directorio de salida si no existe
    int res = mkdir(config.output_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (res != 0 && errno != EEXIST) {
        cerr << "ERROR: No se pudo crear el directorio de salida: " << config.output_path << endl;
        return;
    }

    vector<string> input_files = list_directory(config.input_path);
    if (input_files.empty()) {
        cout << "No se encontraron archivos regulares para procesar en: " << config.input_path << endl;
        return;
    }

    vector<thread> workers;
    for (const string& input_file : input_files) {
        // Extraer el nombre del archivo para la salida
        size_t last_slash = input_file.find_last_of('/');
        string filename = (last_slash == string::npos) ? input_file : input_file.substr(last_slash + 1);
        
        // Agregar sufijo a la salida (esto es simplificado; un sistema robusto usaría metadatos)
        string suffix = (config.compress ? ".lz77" : (config.encrypt ? ".enc" : ""));
        string output_file = config.output_path + "/" + filename + suffix;

        // Lanzar un hilo para cada archivo
        // Usamos std::ref para pasar la configuración por referencia constante
        workers.emplace_back(process_file, input_file, output_file, std::ref(config));
    }

    // Esperar a que todos los hilos terminen (join)
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    cout << "--- Procesamiento concurrente de directorios finalizado ---" << endl;
}

// =================================================================
// PARSEO DE ARGUMENTOS Y FUNCIÓN MAIN
// =================================================================

void show_help(const string& program_name) {
    cout << "Uso: " << program_name << " [OPCIONES] -i <ruta_entrada> -o <ruta_salida>" << endl;
    cout << "\nOperaciones:" << endl;
    cout << "  -c          Comprimir (" << COMP_ALG_LZ77 << ")" << endl;
    cout << "  -d          Descomprimir (" << COMP_ALG_LZ77 << ")" << endl;
    cout << "  -e          Encriptar (" << ENC_ALG_VIGENERE << ")" << endl;
    cout << "  -u          Desencriptar (" << ENC_ALG_VIGENERE << ")" << endl;
    cout << "  NOTA: Las operaciones se realizan en el siguiente orden: Desencriptar -> Descomprimir -> Comprimir -> Encriptar." << endl;
    cout << "\nArgumentos obligatorios:" << endl;
    cout << "  -i <ruta>   Ruta del archivo o directorio de entrada." << endl;
    cout << "  -o <ruta>   Ruta del archivo o directorio de salida." << endl;
    cout << "\nArgumentos opcionales:" << endl;
    cout << "  -k <clave>  Clave secreta para operaciones de encriptación/desencriptación." << endl;
    cout << "  --comp-alg <alg>  Algoritmo de compresión (Actual: " << COMP_ALG_LZ77 << ")." << endl;
    cout << "  --enc-alg <alg>   Algoritmo de encriptación (Actual: " << ENC_ALG_VIGENERE << ")." << endl;
    cout << "  -h          Mostrar esta ayuda." << endl;
}

int main(int argc, char* argv[]) {
    Config config;

    // 1. Recolección de argumentos en un mapa
    map<string, string> args;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg[0] == '-') {
            // Opciones cortas que esperan valor (-i, -o, -k)
            if (arg.size() == 2 && (arg == "-i" || arg == "-o" || arg == "-k")) {
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    args[arg] = argv[i+1];
                    i++; 
                } else {
                    args[arg] = "";
                }
            } else if (arg.substr(0, 2) == "--") {
                 // Opciones largas que esperan valor (--comp-alg, --enc-alg)
                if (i + 1 < argc && argv[i+1][0] != '-') {
                    args[arg] = argv[i+1];
                    i++;
                }
            } else {
                // Opciones cortas sin valor o combinadas (-c, -d, -ce)
                args[arg] = "";
            }
        }
    }

    // 2. Parsear Operaciones y Opciones
    if (args.count("-h")) {
        show_help(argv[0]);
        return 0;
    }

    if (args.count("-i")) config.input_path = args["-i"];
    if (args.count("-o")) config.output_path = args["-o"];
    if (args.count("-k")) config.key = args["-k"];
    if (args.count("--comp-alg")) config.comp_alg = args["--comp-alg"];
    if (args.count("--enc-alg")) config.enc_alg = args["--enc-alg"];

    // Manejo de la cadena de operaciones combinadas (ej: -ce)
    for (const auto& pair : args) {
        if (pair.first.find('-') != string::npos && pair.first.size() > 1) {
            for (char op : pair.first.substr(1)) {
                if (op == 'c') config.compress = true;
                if (op == 'd') config.decompress = true;
                if (op == 'e') config.encrypt = true;
                if (op == 'u') config.decrypt = true;
            }
        }
    }
    
    // 3. Validar Argumentos
    if (config.input_path.empty() || config.output_path.empty()) {
        cerr << "ERROR: Las rutas de entrada (-i) y salida (-o) son obligatorias." << endl;
        show_help(argv[0]);
        return 1;
    }

    if (config.compress && config.decompress) {
        cerr << "ERROR: No se pueden especificar ambas operaciones de compresión y descompresión." << endl;
        return 1;
    }
    if (config.encrypt && config.decrypt) {
        cerr << "ERROR: No se pueden especificar ambas operaciones de encriptación y desencriptación." << endl;
        return 1;
    }
    if ((config.encrypt || config.decrypt) && config.key.empty()) {
        cerr << "ERROR: Se requiere una clave secreta (-k) para las operaciones de encriptación/desencriptación." << endl;
        return 1;
    }

    // 4. Validar Algoritmos Soportados
    if ((config.compress || config.decompress) && config.comp_alg != COMP_ALG_LZ77) {
        cerr << "ERROR: El algoritmo de compresión '" << config.comp_alg << "' no es compatible. Solo se soporta " << COMP_ALG_LZ77 << "." << endl;
        return 1;
    }
    if ((config.encrypt || config.decrypt) && config.enc_alg != ENC_ALG_VIGENERE) {
        cerr << "ERROR: El algoritmo de encriptación '" << config.enc_alg << "' no es compatible. Solo se soporta " << ENC_ALG_VIGENERE << "." << endl;
        return 1;
    }

    // 5. Ejecutar la operación (Archivo único vs. Directorio concurrente)
    if (is_directory(config.input_path)) {
        process_directory(config);
    } else {
        cout << "--- Modo Archivo Único: Iniciando procesamiento secuencial ---" << endl;
        process_file(config.input_path, config.output_path, config);
        cout << "--- Procesamiento de archivo único finalizado ---" << endl;
    }

    return 0;
}