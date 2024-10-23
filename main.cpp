#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

// Constantes para el algoritmo de Rabin-Karp
const int BASE = 256;  // Base utilizada para calcular el hash
const int PRIME = 101; // Número primo para evitar colisiones excesivas

// Función para calcular el hash inicial de una cadena
long computeHash(const std::string& str, int end) {
    long hash = 0;
    for (int i = 0; i < end; ++i)
        hash = (hash * BASE + str[i]) % PRIME;
    return hash;
}

// Función para verificar el plagio entre dos textos utilizando ventanas deslizantes
double checarPlagio(const std::string& mainText, const std::string& compareText, int windowSize) {
    // Si alguno de los textos es más corto que el tamaño de la ventana, no se puede comparar
    if (mainText.length() < windowSize || compareText.length() < windowSize)
        return 0.0;

    // Precalcular (BASE^(windowSize-1)) % PRIME para usar en el cálculo de hashes deslizantes
    long h = 1;
    for (int i = 0; i < windowSize - 1; ++i)
        h = (h * BASE) % PRIME;

    // Calcular los hashes de todas las subcadenas del texto principal
    std::unordered_set<long> mainHashes;
    long mainHash = computeHash(mainText, windowSize);
    mainHashes.insert(mainHash);

    for (size_t i = windowSize; i < mainText.length(); ++i) {
        mainHash = ( (mainHash - mainText[i - windowSize] * h) * BASE + mainText[i] ) % PRIME;
        if (mainHash < 0)
            mainHash += PRIME;
        mainHashes.insert(mainHash);
    }

    // Calcular los hashes del texto a comparar y contar las coincidencias
    long compareHash = computeHash(compareText, windowSize);
    int matchCount = mainHashes.find(compareHash) != mainHashes.end() ? 1 : 0;
    int total = 1;

    for (size_t i = windowSize; i < compareText.length(); ++i) {
        compareHash = ( (compareHash - compareText[i - windowSize] * h) * BASE + compareText[i] ) % PRIME;
        if (compareHash < 0)
            compareHash += PRIME;
        if (mainHashes.find(compareHash) != mainHashes.end())
            matchCount++;
        total++;
    }

    // Calcular el porcentaje de similitud
    return (static_cast<double>(matchCount) / total) * 100.0;
}

int main() {
    std::string mainFilePath;
    int numFiles;
    int windowSize = 10; // Puedes ajustar el tamaño de la ventana según sea necesario

    // Solicitar al usuario la ruta del archivo principal
    std::cout << "Ingrese la ruta del archivo de texto principal: ";
    std::cin >> mainFilePath;

    // Leer el archivo principal
    std::ifstream mainFile(mainFilePath);
    if (!mainFile.is_open()) {
        std::cerr << "Error al abrir el archivo principal.\n";
        return 1;
    }

    // Leer todo el contenido del archivo principal en una cadena
    std::string mainText((std::istreambuf_iterator<char>(mainFile)),
                          std::istreambuf_iterator<char>());
    mainFile.close();

    // Solicitar al usuario el número de archivos a comparar
    std::cout << "Ingrese el número de archivos para comparar: ";
    std::cin >> numFiles;

    // Almacenar las rutas de los archivos a comparar
    std::vector<std::string> compareFiles(numFiles);
    for (int i = 0; i < numFiles; ++i) {
        std::cout << "Ingrese la ruta para el archivo " << i + 1 << ": ";
        std::cin >> compareFiles[i];
    }

    // Comparar cada archivo con el archivo principal
    for (const auto& filePath : compareFiles) {
        std::ifstream compareFile(filePath);
        if (!compareFile.is_open()) {
            std::cerr << "Error al abrir el archivo: " << filePath << "\n";
            continue;
        }

        // Leer todo el contenido del archivo a comparar en una cadena
        std::string compareText((std::istreambuf_iterator<char>(compareFile)),
                                std::istreambuf_iterator<char>());
        compareFile.close();

        // Calcular la similitud entre el archivo principal y el archivo comparado
        double similaridad = checarPlagio(mainText, compareText, windowSize);
        std::cout << "Similitud con \"" << filePath << "\": " << similaridad << "%\n";
    }

    return 0;
}
