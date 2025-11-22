# Proyecto Final - Sistemas Operativos

## Integrantes:
Natalia Ceballos y Eduardo Piñeros

## 1. Introducción: 

El proyecto consiste en el desarrollo de una utilidad de línea de comandos en C++, diseñada para realizar operaciones de compresión/descompresión y encriptación/desencriptación en archivos individuales o directorios completos de forma eficiente.

El principal requisito de diseño es la eficiencia, lograda a través de:

- Concurrencia: Utilización de hilos para procesar múltiples archivos de un directorio simultáneamente, maximizando el uso de sistemas multinúcleo.

- Uso de llamadas directas al sistema operativo en lugar de las abstracciones de alto nivel de la librería estándar <stdio.h>, lo que permite un mayor control sobre la transferencia de datos.

La aplicación es invocada desde la terminal, aceptando parámetros para definir la operación (-c, -d, -e, -u), las rutas de entrada/salida (-i, -o) y la clave secreta (-k).

## 2. Diseño de la Solucion:

<img width="788" alt="image" src="https://github.com/user-attachments/assets/96cc233d-c6d1-46f0-934d-18ac130d25eb" />


## 3. Guia de Uso:
El proyecto requiere un compilador C++ con soporte para la librería de hilos (como g++ en sistemas POSIX).

Compilar todos los archivos fuente (.cpp) y enlazar la librería de hilos (-pthread) 

**Ejecutar:**

make 

./gsea.exe 


El programa debe ser ejecutado desde la terminal con los siguientes parámetros:

- Operación
-c, -d, -e, -u (combinables)

- Entrada
-i [ruta]

- Salida
-o [ruta]

- Clave
-k [clave] (Opcional)

- Algoritmos
--comp-alg LZ77, --enc-alg Vigenere (Predeterminado)

**Ejemplo:**

./gsea.exe -c -e -i img.jpg -o comprimido -k clave123
/gsea.exe -d -u -i comprimido -o descomprimido.jpg -k clave123
