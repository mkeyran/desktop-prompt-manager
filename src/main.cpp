#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJSEngine>
#include <QDir>
#include <QStandardPaths>

#include "database/database.h"
#include "repository/promptrepository.h"
#include "viewmodels/promptlistviewmodel.h"
#include "viewmodels/prompteditviewmodel.h"
#include "viewmodels/placeholderviewmodel.h"
#include "models/prompt.h"
#include "models/folder.h"
#include "utils/placeholderutils.h"
#include "utils/clipboardutils.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("Prompt Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("PromptManager");
    
    // Initialize database
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    QString dbPath = dataPath + "/prompts.db";
    
    Database* database = Database::instance();
    if (!database->initialize(dbPath)) {
        qCritical() << "Failed to initialize database";
        return -1;
    }
    
    // Create repository
    PromptRepository* repository = new PromptRepository(database);
    
    // Create view models and utilities
    PromptListViewModel* promptListViewModel = new PromptListViewModel(repository);
    PromptEditViewModel* promptEditViewModel = new PromptEditViewModel(repository);
    PlaceholderViewModel* placeholderViewModel = new PlaceholderViewModel();
    ClipboardUtils* clipboardUtils = new ClipboardUtils();
    
    // Register QML types
    qmlRegisterType<PromptListViewModel>("PromptManager", 1, 0, "PromptListViewModel");
    qmlRegisterType<PromptEditViewModel>("PromptManager", 1, 0, "PromptEditViewModel");
    qmlRegisterType<PlaceholderViewModel>("PromptManager", 1, 0, "PlaceholderViewModel");
    qmlRegisterType<Prompt>("PromptManager", 1, 0, "Prompt");
    qmlRegisterType<Folder>("PromptManager", 1, 0, "Folder");
    qmlRegisterType<PlaceholderUtils>("PromptManager", 1, 0, "PlaceholderUtils");
    qmlRegisterType<ClipboardUtils>("PromptManager", 1, 0, "ClipboardUtils");
    
    QQmlApplicationEngine engine;
    
    // Expose view models and utilities to QML
    engine.rootContext()->setContextProperty("promptListViewModel", promptListViewModel);
    engine.rootContext()->setContextProperty("promptEditViewModel", promptEditViewModel);
    engine.rootContext()->setContextProperty("placeholderViewModel", placeholderViewModel);
    engine.rootContext()->setContextProperty("clipboardUtils", clipboardUtils);
    
    const QUrl url(QStringLiteral("qrc:/qt/qml/PromptManager/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}