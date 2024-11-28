#include "PlagiarismDetector.h"
#include <iostream>

PlagiarismDetector::PlagiarismDetector(QObject *parent) : QObject(parent), detector(15) {}

void PlagiarismDetector::compareFiles(const QString &mainFile, const QVector<QString> &compareFiles) {
    std::string mainFilePath = mainFile.toStdString();
    if (!detector.cargarArchivoPrincipal(mainFilePath)) {
        emit comparisonResult("Error loading main file", 0);
        return;
    }

    std::vector<std::string> filesToCompare;
    for (const QString &file : compareFiles) {
        filesToCompare.push_back(file.toStdString());
    }

    if (!detector.cargarArchivosComparar(filesToCompare)) {
        emit comparisonResult("Error loading comparison files", 0);
        return;
    }

    results = detector.realizarComparaciones();
    for (const auto &result : results) {
        emit comparisonResult(QString::fromStdString(result.rutaArchivo), result.similitud);
    }
}

void PlagiarismDetector::generateReport(const QString &reportPath) {
    detector.generarReporteHTML(reportPath.toStdString(), results);
    emit reportGenerated(reportPath);
}
