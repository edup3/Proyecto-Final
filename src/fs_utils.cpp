#include "fs_utils.hpp"
#include <iostream>
#include <cstring>

// Cabeceras POSIX para E/S de bajo nivel y manejo de directorios
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

FileData read_file_posix(const string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        cerr << "ERROR: No se pudo abrir el archivo de entrada: " << filename << endl;
        return FileData();
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        cerr << "ERROR: No se pudo obtener el estado del archivo: " << filename << endl;
        close(fd);
        return FileData();
    }

    FileData data(st.st_size);
    ssize_t bytes_read = 0;
    size_t total_read = 0;

    while (total_read < st.st_size) {
        bytes_read = read(fd, data.data() + total_read, st.st_size - total_read);
        if (bytes_read < 0) {
            cerr << "ERROR: Error de lectura del archivo: " << filename << endl;
            close(fd);
            return FileData();
        }
        if (bytes_read == 0) {
            break;
        }
        total_read += bytes_read;
    }

    close(fd);
    if (total_read != st.st_size) {
        cerr << "ADVERTENCIA: Se esperaba leer " << st.st_size << " bytes, pero solo se leyeron " << total_read << endl;
        data.resize(total_read);
    }
    return data;
}

bool write_file_posix(const string& filename, const FileData& data) {
    // O_CREAT | O_TRUNC | Permisos 0600
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        cerr << "ERROR: No se pudo abrir/crear el archivo de salida: " << filename << endl;
        return false;
    }

    size_t total_written = 0;
    while (total_written < data.size()) {
        ssize_t bytes_written = write(fd, data.data() + total_written, data.size() - total_written);
        if (bytes_written < 0) {
            cerr << "ERROR: Error de escritura del archivo: " << filename << endl;
            close(fd);
            return false;
        }
        total_written += bytes_written;
    }

    close(fd);
    return true;
}

bool is_directory(const string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

vector<string> list_directory(const string& path) {
    vector<string> files;
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        cerr << "ERROR: No se pudo abrir el directorio: " << path << endl;
        return files;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Ignorar "." y ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            string full_path = path + "/" + entry->d_name;
            struct stat st;
            // Verificar si es un archivo regular
            if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                files.push_back(full_path);
            }
        }
    }

    closedir(dir);
    return files;
}
