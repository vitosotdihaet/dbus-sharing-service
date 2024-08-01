#include "sharingService.hpp"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QFileInfo>
#include <QtDBus>
#include <QtGlobal>

SharingService::SharingService(const QString &serviceName,
                               const QStringList &supportedFormats,
                               std::function<void(const QString &)> onOpenFile,
                               QObject *parent)
    : QDBusVirtualObject(parent), serviceName(serviceName),
      supportedFormats(supportedFormats), onOpenFile(onOpenFile) {}

bool SharingService::start() {
  return registerService() && registerObject() && addToSharingRegisterService();
}

bool SharingService::registerService() {
  QDBusConnection dbus = QDBusConnection::sessionBus();
  if (!dbus.registerService(serviceName)) {
    qInfo("Can't register D-Bus service.");
    return false;
  }

  return true;
}

bool SharingService::registerObject() {
  QDBusConnection dbus = QDBusConnection::sessionBus();
  if (!dbus.registerVirtualObject("/", this)) {
    qInfo("Can't register D-Bus object.");
    return false;
  }

  return true;
}

bool SharingService::addToSharingRegisterService() {
  QDBusConnection dbus = QDBusConnection::sessionBus();
  if (!dbus.isConnected()) {
    qInfo("Can't connect to the D-Bus session bus.");
    return false;
  }

  QDBusInterface interface("com.system.sharing", "/", "com.system.sharing",
                           dbus); // autoexec if not running
  if (!interface.isValid()) {
    qInfo("The sharing system is not running.");
    return false;
  }

  QDBusMessage reply =
      interface.call("RegisterService", serviceName, supportedFormats);
  if (reply.type() == QDBusMessage::ErrorMessage) {
    qInfo() << "Failed to register service:" << reply.errorMessage();
    return false;
  }

  return true;
}

bool SharingService::OpenFile(const QString &path) {
  QFile file(path);
  if (!file.exists()) {
    qInfo() << "File" << path << "doesn't exist";
    return false;
  }
  onOpenFile(path);
  return true;
}

bool SharingService::handleMessage(const QDBusMessage &message,
                                   const QDBusConnection &dbus) {
  if (message.interface() == serviceName && message.member() == "OpenFile") {
    if (message.arguments().size() != 1) {
      QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs,
                                                     "Invalid arguments");
      dbus.send(error);
      return true;
    }
    QString path = message.arguments().at(0).toString();
    
    QDBusMessage reply;
    if (OpenFile(path)) {
      reply = message.createReply();
    } else {
      reply = message.createErrorReply(QDBusError::InvalidArgs, "File" + path + "doesn't exist");
    }
    dbus.send(reply);
    return true;
  }
  return false;
}

QString SharingService::introspect(const QString &path) const {
  return QString("<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object "
                 "Introspection 1.0//EN\" "
                 "\"http://www.freedesktop.org/standards/dbus/1.0/"
                 "introspect.dtd\">\n"
                 "<node>\n"
                 "  <interface name=\"%1\">\n"
                 "    <method name=\"OpenFile\">\n"
                 "      <arg name=\"%2\" type=\"s\" direction=\"in\"/>\n"
                 "    </method>\n"
                 "  </interface>\n"
                 "</node>\n")
      .arg(serviceName, path);
}
