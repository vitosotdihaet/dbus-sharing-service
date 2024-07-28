#include "sharingService.hpp"

#include <QCoreApplication>
#include <QDBusConnection>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService sharingService;

    QDBusConnection dbusConnection = QDBusConnection::sessionBus();
    if (!dbusConnection.isConnected()) {
        qFatal("Can't connect to the D-Bus session bus.");
        return 1;
    }

    if (!dbusConnection.registerService("com.system.sharing")) {
        qFatal("Can't register D-Bus service.");
        return 1;
    }

    if (!dbusConnection.registerObject("/", &sharingService, QDBusConnection::ExportAllSlots)) {
        qFatal("Can't register D-Bus object.");
        return 1;
    }

    return app.exec();
}
