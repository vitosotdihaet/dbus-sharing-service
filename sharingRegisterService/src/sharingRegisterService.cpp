#include "sharingRegisterService.hpp"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>
#include <QCoreApplication>

#include <cstdlib>
#include <ctime>


SharingRegisterService::SharingRegisterService(QObject *parent) :
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

    loadServices();
}

SharingRegisterService::~SharingRegisterService() {
    saveServices();
}

void SharingRegisterService::RegisterService(const QString &name, const QStringList &supportedFormats) {
    services[name] = supportedFormats;
    saveServices();
}

void SharingRegisterService::OpenFile(const QString &path) {
    QFile file(path);
    if (!file.exists()) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "File does not exist");
        QDBusConnection::sessionBus().send(error);
        return;
    }

    QString fileExtension = QFileInfo(path).suffix().toLower();

    QStringList availableServices;
    for (auto it = services.cbegin(); it != services.cend(); ++it) {
        if (it.value().contains(fileExtension) && isServiceRunning(it.key())) {
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

void SharingRegisterService::OpenFileUsingService(const QString &path, const QString &service) {
    if (!services.contains(service) || !isServiceRunning(service)) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "Service is not registered");
        QDBusConnection::sessionBus().send(error);
        return;
    }

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qFatal("Cannot connect to the D-Bus session bus.");
        return;
    }

    QDBusInterface interface(service, "/", service, connection);
    if (!interface.isValid()) {
        qFatal("D-Bus interface is not valid.");
        return;
    }

    QDBusMessage reply = interface.call("OpenFile", path);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Failed to open the file:" << path << "with service:" << service << "--" << reply.errorMessage();
        return;
    }
}

void SharingRegisterService::loadServices() {
    QSettings settings(configFilePath, QSettings::IniFormat);

    settings.beginGroup("Services");
    QStringList keys = settings.childKeys();
    for (const QString &key : keys) {
        services[key] = settings.value(key).toStringList();
    }
    settings.endGroup();
}

void SharingRegisterService::saveServices() {
    QSettings settings(configFilePath, QSettings::IniFormat);

    settings.beginGroup("Services");
    for (const QString &key : services.keys()) {
        settings.setValue(key, services[key]);
    }
    settings.endGroup();
}

bool SharingRegisterService::isServiceRunning(const QString &service) {
    QDBusConnection dbusConnection = QDBusConnection::sessionBus();
    if (dbusConnection.interface()->isServiceRegistered(service)) {
        return true;
    }
    return false;
}
