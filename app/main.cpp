#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "include/filesmodel.h"
#include "include/filerenamer.h"
#include "include/urltopathformatter.h"
#include "include/qdirectorychangescontroller.h"

static QObject * qobject_singletonurlformatter_provider(QQmlEngine* engine = NULL, QJSEngine* scriptEngine = NULL)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto instance = new UrlToPathFormatter();

    return instance;
}

static QObject * qobject_singletonfilerenamer_provider(QQmlEngine* engine = NULL, QJSEngine* scriptEngine = NULL)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto instance = new FileRenamer();

    return instance;
}

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<QDirectoryChangesController>("DirectoryChangesWatcher", 1, 0, "DirectoryChangesController");
    qmlRegisterType<FilesModel>("DirectoryChangesWatcher", 1, 0, "FilesModel");
    qmlRegisterSingletonType<UrlToPathFormatter>("DirectoryChangesWatcher", 1, 0, "UrlFormatter", qobject_singletonurlformatter_provider);
    qmlRegisterSingletonType<UrlToPathFormatter>("DirectoryChangesWatcher", 1, 0, "FileRenamer", qobject_singletonfilerenamer_provider);
    qmlRegisterUncreatableType<FileInfoWrapper>("DirectoryChangesWatcher", 1, 0, "FileInfo", "whatever");

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml");
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
