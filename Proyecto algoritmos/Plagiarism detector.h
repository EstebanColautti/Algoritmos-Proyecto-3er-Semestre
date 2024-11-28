#ifndef PLAGIARISMDETECTOR_H
#define PLAGIARISMDETECTOR_H

#include <QObject>
#include <QString>
#include <QVector>

class PlagiarismDetector : public QObject {
    Q_OBJECT
public:
    explicit PlagiarismDetector(QObject *parent = nullptr);

    Q_INVOKABLE void compareFiles(const QString &mainFile, const QVector<QString> &compareFiles);
    Q_INVOKABLE void generateReport(const QString &reportPath);

signals:
    void comparisonResult(QString filePath, double similarity);
    void reportGenerated(QString reportPath);

private:
    std::vector<ResultadoComparacion> results;
    DetectorPlagio detector;
};

#endif // PLAGIARISMDETECTOR_H
