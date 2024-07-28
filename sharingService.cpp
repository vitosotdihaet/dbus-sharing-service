#include "sharingService.hpp"

#include <QObject>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QProcess>

#include <cstdlib>
#include <ctime>
#include <filesystem>



SharingService::SharingService(QObject *parent) :
    QObject(parent) {
    QDBusConnection dbusConnection = QDBusConnection::sessionBus();

    if (!dbusConnection.isConnected()) {
        qFatal("Can't connect to the D-Bus session bus.");
        return;
    }

    if (dbusConnection.interface()->isServiceRegistered(QString(DBUS_SERVICE_NAME))) {
        qFatal("Service is already registered.");
        return;
    }

    if (!dbusConnection.registerService(QString(DBUS_SERVICE_NAME))) {
        qFatal("Can't register D-Bus service.");
        return;
    }

    if (!dbusConnection.registerObject("/", this, QDBusConnection::ExportAllSlots)) {
        qFatal("Can't register D-Bus object.");
        return;
    }

    std::srand(std::time(nullptr)); // set the random seed for random service selection
}

void SharingService::RegisterService(const QString &name, const QStringList &supportedFormats) {
    if (services.contains(name)) {
        services[name] = supportedFormats;
    } else {
        services.insert(name, supportedFormats);
    }
}

void SharingService::OpenFile(const QString &path) {
    QFile file(path);
    if (!file.exists()) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "File does not exist");
        QDBusConnection::sessionBus().send(error);
        return;
    }

    QString fileExtension = QFileInfo(path).suffix().toLower();

    QStringList availableServices;
    for (auto it = services.cbegin(); it != services.cend(); ++it) {
        if (it.value().contains(fileExtension)) {
            availableServices.append(it.key());
        }
    }

    if (availableServices.isEmpty()) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "No available services to open the file");
        QDBusConnection::sessionBus().send(error);
        return;
    }

    QString selectedService = availableServices.at(std::rand() % availableServices.size());
    OpenFileUsingService(path, selectedService);
}

void SharingService::OpenFileUsingService(const QString &path, const QString &service) {
    if (!services.contains(service)) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "Service is not registered");
        QDBusConnection::sessionBus().send(error);
        return;
    }

    QProcess::startDetached(service, QStringList() << path);
}