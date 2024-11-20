#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <sstream>

// Definicion de hash doble
struct HashDoble {
    unsigned long long hash1;
    unsigned long long hash2;

    bool operator==(const HashDoble& otro) const {
        return hash1 == otro.hash1 && hash2 == otro.hash2;
    }
};

// Especializacion de std::hash para HashDoble
namespace std {
    template <>
    struct hash<HashDoble> {
        size_t operator()(const HashDoble& hd) const {
            return hd.hash1 ^ hd.hash2;
        }
    };
}

namespace fs = std::filesystem;

// Constantes para el algoritmo de Rabin-Karp
const unsigned long long BASE1 = 131;
const unsigned long long PRIME1 = 1000000007;
const unsigned long long BASE2 = 137;
const unsigned long long PRIME2 = 1000000009;

// Estructura para almacenar resultados de comparacion
struct ResultadoComparacion {
    std::string rutaArchivo;
    double similitud;
};

// Funcion para limpiar y estandarizar la ruta de archivo
std::string limpiarRutaArchivo(const std::string& ruta) {
    std::string rutaLimpia = ruta;
    // Eliminar comillas iniciales y finales si existen
    if (!rutaLimpia.empty() && rutaLimpia.front() == '\"' && rutaLimpia.back() == '\"') {
        rutaLimpia = rutaLimpia.substr(1, rutaLimpia.size() - 2);
    }
    return rutaLimpia;
}

// Funcion para limpiar texto de puntuacion y espacios extra
std::string limpiarTexto(const std::string& texto) {
    std::string textoLimpio;
    for (char c : texto) {
        if (isalnum(static_cast<unsigned char>(c))) {
            textoLimpio += tolower(static_cast<unsigned char>(c));
        } else if (isspace(static_cast<unsigned char>(c))) {
            if (textoLimpio.empty() || textoLimpio.back() != ' ') {
                textoLimpio += ' ';
            }
        }
    }
    return textoLimpio;
}

// Funcion para calcular el hash inicial doble
HashDoble calcularHash(const std::string& str, int fin) {
    unsigned long long hash1 = 0;
    unsigned long long hash2 = 0;
    for (int i = 0; i < fin; ++i) {
        hash1 = (hash1 * BASE1 + str[i]) % PRIME1;
        hash2 = (hash2 * BASE2 + str[i]) % PRIME2;
    }
    return {hash1, hash2};
}

// Funcion para calcular el hash deslizante doble
HashDoble recalcularHash(const HashDoble& hashAntiguo, char charAntiguo, char charNuevo, unsigned long long h1, unsigned long long h2) {
    unsigned long long nuevoHash1 = (BASE1 * (hashAntiguo.hash1 + PRIME1 - (charAntiguo * h1 % PRIME1)) + charNuevo) % PRIME1;
    unsigned long long nuevoHash2 = (BASE2 * (hashAntiguo.hash2 + PRIME2 - (charAntiguo * h2 % PRIME2)) + charNuevo) % PRIME2;
    return {nuevoHash1, nuevoHash2};
}

class DetectorPlagio {
public:
    // Constructor con tamano de ventana predeterminado
    DetectorPlagio(int tamanoVentana = 15)
        : tamanoVentana(tamanoVentana) {}

    // Cargar el archivo principal
    bool cargarArchivoPrincipal(const std::string& rutaArchivo) {
        std::string contenido = leerContenidoArchivo(rutaArchivo);
        if (contenido.empty()) {
            std::cerr << "Error al leer el archivo principal: " << rutaArchivo << "\n";
            return false;
        }
        textoPrincipal = limpiarTexto(contenido);
        return true;
    }

    // Cargar los archivos para comparar
    bool cargarArchivosComparar(const std::vector<std::string>& rutasArchivos) {
        bool todosArchivosValidos = true;
        for (const auto& ruta : rutasArchivos) {
            std::string rutaLimpia = limpiarRutaArchivo(ruta);
            if (fs::exists(rutaLimpia) && fs::is_regular_file(rutaLimpia)) {
                archivosComparar.push_back(rutaLimpia);
                std::cout << "Archivo para comparar cargado: " << rutaLimpia << "\n";
            } else {
                std::cerr << "Archivo no encontrado o invalido: " << rutaLimpia << "\n";
                todosArchivosValidos = false;
            }
        }
        return !archivosComparar.empty();
    }

    // Metodo publico para realizar todas las comparaciones y obtener los resultados
    std::vector<ResultadoComparacion> realizarComparaciones() {
        std::vector<ResultadoComparacion> resultados;
        for (const auto& rutaArchivo : archivosComparar) {
            std::string textoComparar = leerContenidoArchivo(rutaArchivo);
            if (textoComparar.empty()) {
                std::cerr << "Error al leer el archivo: " << rutaArchivo << "\n";
                continue;
            }
            textoComparar = limpiarTexto(textoComparar);
            double similitud = verificarPlagio(textoComparar);
            resultados.push_back({rutaArchivo, similitud});
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Similitud con \"" << rutaArchivo << "\": " << similitud << "%\n";
        }
        return resultados;
    }

    // Generar el reporte HTML
    void generarReporteHTML(const std::string& rutaReporte, const std::vector<ResultadoComparacion>& resultados) {
        std::ofstream archivoReporte(rutaReporte);
        if (!archivoReporte) {
            std::cerr << "No se pudo crear el archivo de reporte: " << rutaReporte << "\n";
            return;
        }

        archivoReporte << R"(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Reporte de Plagio</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f6f8;
            margin: 0;
            padding: 20px;
        }
        h1 {
            text-align: center;
            color: #333;
        }
        .summary {
            margin: 20px auto;
            width: 80%;
            background-color: #fff;
            padding: 15px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
        }
        th, td {
            padding: 12px;
            text-align: left;
        }
        th {
            background-color: #007BFF;
            color: white;
        }
        tr:nth-child(even) {
            background-color: #f2f2f2;
        }
        .percentage {
            font-weight: bold;
        }
        .high {
            color: #d9534f;
        }
        .medium {
            color: #f0ad4e;
        }
        .low {
            color: #5cb85c;
        }
    </style>
</head>
<body>
    <h1>Reporte de Plagio</h1>
    <div class="summary">
        <table>
            <tr>
                <th>Archivo Comparado</th>
                <th>Similitud (%)</th>
            </tr>
)" ;

        for (const auto& resultado : resultados) {
            std::string claseNombre;
            if (resultado.similitud >= 70.0) {
                claseNombre = "high";
            } else if (resultado.similitud >= 40.0) {
                claseNombre = "medium";
            } else {
                claseNombre = "low";
            }

            archivoReporte << "            <tr>\n";
            archivoReporte << "                <td>" << resultado.rutaArchivo << "</td>\n";
            archivoReporte << "                <td class=\"percentage " << claseNombre << "\">"
                          << std::fixed << std::setprecision(2) << resultado.similitud << "%</td>\n";
            archivoReporte << "            </tr>\n";
        }

        archivoReporte << R"(
        </table>
    </div>
</body>
</html>
)";
        archivoReporte.close();
        std::cout << "Reporte generado exitosamente: " << rutaReporte << "\n";
    }

private:
    std::string textoPrincipal;
    std::vector<std::string> archivosComparar;
    int tamanoVentana;

    // Funcion para leer contenido de archivo
    std::string leerContenidoArchivo(const std::string& rutaArchivo) {
        std::ifstream archivo(rutaArchivo, std::ios::in);
        if (!archivo) {
            std::cerr << "No se pudo abrir el archivo: " << rutaArchivo << "\n";
            return "";
        }
        std::ostringstream ss;
        ss << archivo.rdbuf();
        return ss.str();
    }

    // Funcion para verificar el plagio
    double verificarPlagio(const std::string& textoComparar) {
        if (textoPrincipal.length() < tamanoVentana || textoComparar.length() < tamanoVentana) return 0.0;

        // Calcular h1 y h2
        unsigned long long h1 = 1, h2 = 1;
        for (int i = 0; i < tamanoVentana - 1; ++i) {
            h1 = (h1 * BASE1) % PRIME1;
            h2 = (h2 * BASE2) % PRIME2;
        }

        // Crear conjuntos de hashes unicos
        std::unordered_set<HashDoble> hashesPrincipales;
        HashDoble hashPrincipal = calcularHash(textoPrincipal, tamanoVentana);
        hashesPrincipales.insert(hashPrincipal);

        // Calcular y almacenar hashes de substrings en textoPrincipal
        for (size_t i = tamanoVentana; i < textoPrincipal.length(); ++i) {
            hashPrincipal = recalcularHash(hashPrincipal, textoPrincipal[i - tamanoVentana], textoPrincipal[i], h1, h2);
            hashesPrincipales.insert(hashPrincipal);
        }

        // Comparar hashes en textoComparar y contar coincidencias
        HashDoble hashComparar = calcularHash(textoComparar, tamanoVentana);
        int contadorCoincidencias = hashesPrincipales.count(hashComparar);
        int totalSubcadenas = 1;

        for (size_t i = tamanoVentana; i < textoComparar.length(); ++i) {
            hashComparar = recalcularHash(hashComparar, textoComparar[i - tamanoVentana], textoComparar[i], h1, h2);
            if (hashesPrincipales.count(hashComparar)) {
                contadorCoincidencias++;
            }
            totalSubcadenas++;
        }

        return totalSubcadenas > 0 ? (static_cast<double>(contadorCoincidencias) / totalSubcadenas) * 100.0 : 0.0;
    }
};

int main() {
    int tamanoVentana;
    std::cout << "===== Detector de Plagio =====\n\n";

    // Entrada de tamano de ventana
    std::cout << "Ingrese el tamano de la ventana (numero de caracteres a comparar en cada segmento, por ejemplo, 15): ";
    std::cin >> tamanoVentana;
    std::cin.ignore();

    DetectorPlagio detector(tamanoVentana);
    std::string rutaArchivoPrincipal;
    int numeroArchivos;

    // Entrada de archivo principal
    std::cout << "Ingrese la ruta del archivo de texto principal: ";
    std::getline(std::cin, rutaArchivoPrincipal);
    rutaArchivoPrincipal = limpiarRutaArchivo(rutaArchivoPrincipal);

    // Cargar archivo principal
    if (!detector.cargarArchivoPrincipal(rutaArchivoPrincipal)) {
        std::cerr << "Terminando el programa debido a errores en el archivo principal.\n";
        return 1;
    }

    // Entrada de numero de archivos para comparar
    std::cout << "Ingrese el numero de archivos para comparar: ";
    std::cin >> numeroArchivos;
    std::cin.ignore();

    std::vector<std::string> rutasArchivosComparar;
    for (int i = 0; i < numeroArchivos; ++i) {
        std::string rutaComparar;
        std::cout << "Ingrese la ruta para el archivo " << i + 1 << ": ";
        std::getline(std::cin, rutaComparar);
        rutaComparar = limpiarRutaArchivo(rutaComparar);
        rutasArchivosComparar.push_back(rutaComparar);
    }

    // Cargar archivos para comparar
    if (!detector.cargarArchivosComparar(rutasArchivosComparar)) {
        std::cerr << "No se cargaron archivos para comparar. Terminando el programa.\n";
        return 1;
    }

    // Realizar comparaciones
    std::vector<ResultadoComparacion> resultados = detector.realizarComparaciones();

    // Generar reporte HTML
    std::string rutaReporte = "reporte_plagio.html";
    detector.generarReporteHTML(rutaReporte, resultados);

    return 0;
}
