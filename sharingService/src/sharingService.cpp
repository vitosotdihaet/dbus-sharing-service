#include "sharingService.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtDBus>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

SharingService::SharingService(const QString &serviceName, const QStringList &supportedFormats, std::function<void(const QString &)> onOpenFile, QObject *parent)
    : QDBusVirtualObject(parent), serviceName(serviceName), supportedFormats(supportedFormats), onOpenFile(onOpenFile) {}

bool SharingService::start() {
    return registerService() && registerObject() && addToSharingRegisterService();
}

bool SharingService::registerService() {
    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerService(serviceName)) {
        qFatal("Can't register D-Bus service.");
        return false;
    }

    return true;
}

bool SharingService::registerObject() {
    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerVirtualObject("/", this)) {
        qFatal("Can't register D-Bus object.");
        return false;
    }

    return true;
}

bool SharingService::addToSharingRegisterService() {
    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qFatal("Can't connect to the D-Bus session bus.");
        return false;
    }

    QDBusInterface interface("com.system.sharing", "/", "com.system.sharing", connection); // autoexec if not running
    if (!interface.isValid()) {
        qFatal("The sharing system is not running.");
        return false;
    }

    QDBusMessage reply = interface.call("RegisterService", serviceName, supportedFormats);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Failed to register service:" << reply.errorMessage();
        return false;
    }

    return true;
}

void SharingService::OpenFile(const QString &path) {
    QFile file(path);
    if (!file.exists()) {
        QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "File does not exist");
        QDBusConnection::sessionBus().send(error);
        return;
    }
    onOpenFile(path);
}

bool SharingService::handleMessage(const QDBusMessage &message, const QDBusConnection &connection) {
    if (message.interface() == serviceName && message.member() == "OpenFile") {
        if (message.arguments().size() != 1) {
            QDBusMessage error = QDBusMessage::createError(QDBusError::InvalidArgs, "Invalid arguments");
            connection.send(error);
            return true;
        }
        QString path = message.arguments().at(0).toString();
        OpenFile(path);
        QDBusMessage reply = message.createReply();
        connection.send(reply);
        return true;
    }
    return false;
}

QString SharingService::introspect(const QString &path) const {
    return QString(
        "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
        "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
        "<node>\n"
        "  <interface name=\"%1\">\n"
        "    <method name=\"OpenFile\">\n"
        "      <arg name=\"%2\" type=\"s\" direction=\"in\"/>\n"
        "    </method>\n"
        "  </interface>\n"
        "</node>\n"
    ).arg(serviceName, path);
}
