#include "sharingRegisterService.hpp"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include <cstdlib>
#include <ctime>

SharingRegisterService::SharingRegisterService(QObject *parent)
    : QObject(parent) {
  QDBusConnection dbus = QDBusConnection::sessionBus();

  if (!dbus.isConnected()) {
    qFatal("Can't connect to the D-Bus session bus.");
    return;
  }

  if (dbus.interface()->isServiceRegistered(QString(DBUS_SERVICE_NAME))) {
    qFatal("Service is already registered.");
    return;
  }

  if (!dbus.registerService(QString(DBUS_SERVICE_NAME))) {
    qFatal("Can't register D-Bus service.");
    return;
  }

  if (!dbus.registerObject("/", this, QDBusConnection::ExportAllSlots)) {
    qFatal("Can't register D-Bus object.");
    return;
  }

  std::srand(
      std::time(nullptr)); // set the random seed for random service selection

  loadServices();
}

SharingRegisterService::~SharingRegisterService() { saveServices(); }

void SharingRegisterService::RegisterService(
    const QString &name, const QStringList &supportedFormats) {
  services[name] = supportedFormats;
  saveServices();
}

void SharingRegisterService::OpenFile(const QString &path) {
  QString fileExtension = QFileInfo(path).suffix().toLower();

  QStringList availableServices;
  for (auto it = services.cbegin(); it != services.cend(); ++it) {
    if (it.value().contains(fileExtension) && isServiceRunning(it.key())) {
      availableServices.append(it.key());
    }
  }

  if (availableServices.isEmpty()) {
    QDBusMessage error;
    sendErrorReply(QDBusError::InvalidArgs,
                            "No available services to open " + path);
    return;
  }

  QString selectedService =
      availableServices.at(std::rand() % availableServices.size());
  if (!openFileUsingService(path, selectedService)) {
    QDBusMessage error;
    sendErrorReply(QDBusError::InvalidArgs, "Could not open " + path);
    return;
  }
}

void SharingRegisterService::OpenFileUsingService(const QString &path,
                                                  const QString &service) {
  if (!openFileUsingService(path, service)) {
    QDBusMessage error;
    sendErrorReply(QDBusError::InvalidArgs, "Could not send " + path + " to " + service);
    return;
  }
}

bool SharingRegisterService::openFileUsingService(const QString &path,
                                                  const QString &service) {
  QFile file(path);
  if (!file.exists()) {
    return false;
  }
  if (!services.contains(service) || !isServiceRunning(service)) {
    return false;
  }

  QDBusConnection dbus = QDBusConnection::sessionBus();
  if (!dbus.isConnected()) {
    qFatal("Cannot connect to the D-Bus session bus.");
    return false;
  }

  QDBusInterface interface(service, "/", service, dbus);
  if (!interface.isValid()) {
    qFatal("D-Bus interface is not valid.");
    return false;
  }

  QDBusMessage reply = interface.call("OpenFile", path);
  if (reply.type() == QDBusMessage::ErrorMessage) {
    qDebug() << "Failed to open the file:" << path << "with service:" << service
             << "--" << reply.errorMessage();
    return false;
  }

  return true;
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
  QDBusConnection dbus = QDBusConnection::sessionBus();
  if (dbus.interface()->isServiceRegistered(service)) {
    return true;
  }
  return false;
}
