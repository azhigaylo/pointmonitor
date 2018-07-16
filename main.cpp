#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "dataclient/DataClientInterface.hpp"
#include "models/Conditions.hpp"
#include "models/DataModels.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    CDataClientInterface data_client;
    QObject::connect(&data_client, SIGNAL(dataConnection(bool)), &ConditionsBroker::getInstance(), SLOT(connectionChanged_slot(bool)));
    QObject::connect(&ConditionsBroker::getInstance(), SIGNAL(connectionRequest_signal(bool, QString, int)), &data_client, SLOT(startDataConnection_slot(bool, QString, int)));

    // Register our component type with QML.
    qmlRegisterType<ConnectionModels>("com.ics.hb", 1, 0, "ConnectionModels");
    qmlRegisterType<ConnectionButton>("com.ics.hb", 1, 0, "ConnectionButton");

    QQmlApplicationEngine engine;
    CDiscretPointsModel discret_model(data_client);
    CAnalogPointsModel analog_model(data_client);
    engine.rootContext()->setContextProperty("discret_model", &discret_model);
    engine.rootContext()->setContextProperty("analog_model", &analog_model);

    CPointsUpdateModel update_model(data_client);
    engine.rootContext()->setContextProperty("update_model", &update_model);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) return -1;

    return app.exec();
}
