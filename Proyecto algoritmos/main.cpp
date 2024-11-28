#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "PlagiarismDetector.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<PlagiarismDetector>("Plagiarism", 1, 0, "PlagiarismDetector");

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
