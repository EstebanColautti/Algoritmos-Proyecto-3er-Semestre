#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <filesystem>

// Constantes para el algoritmo de Rabin-Karp
const int BASE = 256;  // Base utilizada para calcular el hash
const int PRIME = 101; // Número primo para evitar colisiones excesivas

// Función para calcular el hash inicial de una cadena (hash modular manual)
long computeHash(const std::string& str, int end) {
    long hash = 0;
    for (int i = 0; i < end; ++i)
        hash = (hash * BASE + str[i]) % PRIME;
    return hash;
}

// Función para calcular el hash deslizante de una subcadena
long recalculateHash(long oldHash, char oldChar, char newChar, long h) {
    long newHash = (oldHash - oldChar * h) * BASE + newChar;
    newHash %= PRIME;//Modulos Derivados del Inicial
    if (newHash < 0)
        newHash += PRIME;
    return newHash;
}

// Función para verificar el plagio entre dos textos
double checarPlagio(const std::string& mainText, const std::string& compareText, int windowSize) {
    if (mainText.length() < windowSize || compareText.length() < windowSize)
        return 0.0;

    long h = 1;
    for (int i = 0; i < windowSize - 1; ++i)
        h = (h * BASE) % PRIME;//Modulo Original del Texto

    std::unordered_set<long> mainHashes;
    long mainHash = computeHash(mainText, windowSize);
    mainHashes.insert(mainHash);

    for (size_t i = windowSize; i < mainText.length(); ++i) {
        mainHash = recalculateHash(mainHash, mainText[i - windowSize], mainText[i], h);//Texto Hash Texto Previo, Texto Previo, Texto Nuevo, Modulo Original del Texto
        mainHashes.insert(mainHash);
    }

    long compareHash = computeHash(compareText, windowSize);
    int matchCount = mainHashes.find(compareHash) != mainHashes.end() ? 1 : 0;
    int total = 1;

    for (size_t i = windowSize; i < compareText.length(); ++i) {
        compareHash = recalculateHash(compareHash, compareText[i - windowSize], compareText[i], h);
        if (mainHashes.find(compareHash) != mainHashes.end())
            matchCount++;
        total++;
    }

    return (static_cast<double>(matchCount) / total) * 100.0;
}

// Función para leer archivos PDF y extraer texto
std::string extractTextFromPDF(const std::string& filePath) {
    // Implementa o utiliza la biblioteca de PDF para extraer texto
    std::string text;
    // Código de extracción de texto aquí
    return text;
}

// Función para leer un archivo, soporta txt y pdf
std::string readFileContent(const std::string& filePath) {
    if (filePath.substr(filePath.find_last_of(".") + 1) == "pdf") {
        return extractTextFromPDF(filePath);
    } else {
        std::ifstream file(filePath);
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
}

int main() {
    std::string mainFilePath;
    int numFiles;
    int windowSize = 10;//Tamano de cada bloque que se hashea la comparacion

    std::cout << "Ingrese la ruta del archivo de texto principal: ";
    std::cin >> mainFilePath;

    std::string mainText = readFileContent(mainFilePath);
    if (mainText.empty()) {
        std::cerr << "Error al leer el archivo principal.\n";
        return 1;
    }

    std::cout << "Ingrese el número de archivos para comparar: ";
    std::cin >> numFiles;

    std::vector<std::string> compareFiles(numFiles);
    for (int i = 0; i < numFiles; ++i) {
        std::cout << "Ingrese la ruta para el archivo " << i + 1 << ": ";
        std::cin >> compareFiles[i];
    }

    for (const auto& filePath : compareFiles) {
        std::string compareText = readFileContent(filePath);
        if (compareText.empty()) {
            std::cerr << "Error al leer el archivo: " << filePath << "\n";
            continue;
        }

        double similaridad = checarPlagio(mainText, compareText, windowSize);
        std::cout << "Similitud con \"" << filePath << "\": " << similaridad << "%\n";
    }

    return 0;
}
