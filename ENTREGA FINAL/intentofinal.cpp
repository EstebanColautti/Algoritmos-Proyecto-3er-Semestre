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
    DetectorPlagio(int tamanoVentana = 15) : tamanoVentana(tamanoVentana) {}

    bool cargarArchivoPrincipal(const std::string& rutaArchivo) {
        std::string contenido = leerContenidoArchivo(rutaArchivo);
        if (contenido.empty()) {
            std::cerr << "Error al leer el archivo principal: " << rutaArchivo << "\n";
            return false;
        }
        textoPrincipal = limpiarTexto(contenido);
        return true;
    }

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

    void generarReporteHTML(const std::string& rutaReporte, const std::vector<ResultadoComparacion>& resultados) {
        std::ofstream archivoReporte(rutaReporte);
        if (!archivoReporte) {
            std::cerr << "No se pudo crear el archivo de reporte: " << rutaReporte << "\n";
            return;
        }
        archivoReporte << R"(<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Reporte de Plagio</title>
    <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap" rel="stylesheet">
    <style>
        body {
            background-color: #121212;
            color: #e0e0e0;
            font-family: 'Roboto', sans-serif;
            margin: 0;
            padding: 0;
        }
        header {
            background: linear-gradient(135deg, #bb86fc, #3700b3);
            padding: 20px;
            text-align: center;
            box-shadow: 0 2px 4px rgba(0,0,0,0.5);
        }
        header h1 {
            margin: 0;
            font-size: 2em;
            color: #ffffff;
        }
        main {
            padding: 20px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        table thead {
            background-color: #1f1f1f;
        }
        table th, table td {
            padding: 12px 15px;
            text-align: left;
        }
        table tbody tr:nth-child(even) {
            background-color: #2c2c2c;
        }
        table tbody tr:hover {
            background-color: #3a3a3a;
            transition: background-color 0.3s ease;
        }
        table th {
            color: #bb86fc;
            font-weight: 700;
            position: relative;
        }
        /* Barra de Similitud */
        .barra-similitud {
            position: relative;
            width: 100%;
            height: 20px;
            background-color: #2c2c2c;
            border-radius: 10px;
            overflow: hidden;
        }
        .indicador {
            height: 100%;
            width: 0%;
            background-color: #bb86fc; /* Color morado */
            border-radius: 10px 0 0 10px;
            transition: width 0.5s ease;
        }
        .porcentaje {
            position: absolute;
            top: 0;
            left: 50%;
            transform: translateX(-50%);
            height: 100%;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #ffffff;
            font-weight: bold;
            pointer-events: none;
            font-size: 0.9em;
        }
        @media (max-width: 600px) {
            table th, table td {
                padding: 8px 10px;
            }
            header h1 {
                font-size: 1.5em;
            }
            .barra-similitud {
                height: 15px;
            }
            .porcentaje {
                font-size: 0.8em;
            }
        }
    </style>
</head>
<body>
    <header>
        <h1>Reporte de Plagio</h1>
    </header>
    <main>
        <table>
            <thead>
                <tr>
                    <th>Archivo</th>
                    <th>Similitud</th>
                </tr>
            </thead>
            <tbody>
)";

        for (const auto& resultado : resultados) {
            // Limitar el porcentaje a 100% y evitar valores negativos
            double porcentaje = resultado.similitud;
            if (porcentaje > 100.0) porcentaje = 100.0;
            if (porcentaje < 0.0) porcentaje = 0.0;

            archivoReporte << "<tr><td>" << resultado.rutaArchivo << "</td><td>"
                          << "<div class=\"barra-similitud\">"
                          << "<div class=\"indicador\" style=\"width: " << porcentaje << "%;\"></div>"
                          << "<span class=\"porcentaje\">" << std::fixed << std::setprecision(2) << porcentaje << "%</span>"
                          << "</div>"
                          << "</td></tr>\n";
        }

        archivoReporte << R"(            </tbody>
        </table>
    </main>
</body>
</html>)";

        archivoReporte.close();
        std::cout << "Reporte generado exitosamente: " << rutaReporte << "\n";
    }

private:
    std::string textoPrincipal;
    std::vector<std::string> archivosComparar;
    int tamanoVentana;

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

    double verificarPlagio(const std::string& textoComparar) {
        if (textoPrincipal.length() < tamanoVentana || textoComparar.length() < tamanoVentana) return 0.0;
        unsigned long long h1 = 1, h2 = 1;
        for (int i = 0; i < tamanoVentana - 1; ++i) {
            h1 = (h1 * BASE1) % PRIME1;
            h2 = (h2 * BASE2) % PRIME2;
        }
        std::unordered_set<HashDoble> hashesPrincipales;
        HashDoble hashPrincipal = calcularHash(textoPrincipal, tamanoVentana);
        hashesPrincipales.insert(hashPrincipal);
        for (size_t i = tamanoVentana; i < textoPrincipal.length(); ++i) {
            hashPrincipal = recalcularHash(hashPrincipal, textoPrincipal[i - tamanoVentana], textoPrincipal[i], h1, h2);
            hashesPrincipales.insert(hashPrincipal);
        }
        HashDoble hashComparar = calcularHash(textoComparar, tamanoVentana);
        int coincidencias = hashesPrincipales.count(hashComparar);
        int totalSubcadenas = 1;
        for (size_t i = tamanoVentana; i < textoComparar.length(); ++i) {
            hashComparar = recalcularHash(hashComparar, textoComparar[i - tamanoVentana], textoComparar[i], h1, h2);
            if (hashesPrincipales.count(hashComparar)) {
                coincidencias++;
            }
            totalSubcadenas++;
        }
        return totalSubcadenas > 0 ? (static_cast<double>(coincidencias) / totalSubcadenas) * 100.0 : 0.0;
    }
};

void mostrarMenu() {
    std::cout << "\n===== Detector de Plagio =====\n";
    std::cout << "1. Cargar archivo principal\n";
    std::cout << "2. Agregar archivos para comparar\n";
    std::cout << "3. Iniciar deteccion de plagio\n";
    std::cout << "4. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main() {
    DetectorPlagio detector(15);
    std::string rutaArchivoPrincipal;
    std::vector<std::string> rutasArchivosComparar;
    std::vector<ResultadoComparacion> resultados;
    bool archivoPrincipalCargado = false;
    int opcion;

    do {
        mostrarMenu();
        std::cin >> opcion;
        std::cin.ignore();

        switch (opcion) {
            case 1: {
                std::cout << "Ingrese la ruta del archivo principal: ";
                std::getline(std::cin, rutaArchivoPrincipal);
                rutaArchivoPrincipal = limpiarRutaArchivo(rutaArchivoPrincipal);
                if (detector.cargarArchivoPrincipal(rutaArchivoPrincipal)) {
                    std::cout << "Archivo principal cargado correctamente.\n";
                    archivoPrincipalCargado = true;
                } else {
                    std::cerr << "Error al cargar el archivo principal.\n";
                }
                break;
            }
            case 2: {
                int cantidad;
                std::cout << "Ingrese el numero de archivos para comparar: ";
                std::cin >> cantidad;
                std::cin.ignore();
                for (int i = 0; i < cantidad; ++i) {
                    std::string ruta;
                    std::cout << "Ingrese la ruta del archivo " << (i + 1) << ": ";
                    std::getline(std::cin, ruta);
                    rutasArchivosComparar.push_back(limpiarRutaArchivo(ruta));
                }
                if (!detector.cargarArchivosComparar(rutasArchivosComparar)) {
                    std::cerr << "Error al cargar algunos archivos para comparar.\n";
                }
                break;
            }
            case 3: {
                if (!archivoPrincipalCargado) {
                    std::cerr << "Debe cargar un archivo principal primero.\n";
                    break;
                }
                if (rutasArchivosComparar.empty()) {
                    std::cerr << "Debe agregar archivos para comparar primero.\n";
                    break;
                }
                resultados = detector.realizarComparaciones();

                // Obtener la ruta del directorio actual donde esta el ejecutable
                fs::path directorioActual = fs::current_path();
                fs::path rutaReporte = directorioActual / "reporte_plagio.html";

                // Generar el reporte HTML automaticamente
                detector.generarReporteHTML(rutaReporte.string(), resultados);
                break;
            }
            case 4:
                std::cout << "Saliendo del programa...\n";
                break;
            default:
                std::cerr << "Opcion no valida.\n";
        }
    } while (opcion != 4);

    return 0;
}
