#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#define DBUS_SERVICE_NAME "com.system.sharing"

class SharingRegisterService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DBUS_SERVICE_NAME)

public:
    explicit SharingRegisterService(QObject *parent = nullptr);
    ~SharingRegisterService();

public slots:
    void RegisterService(const QString &name, const QStringList &supportedFormats);
    void OpenFile(const QString &path);
    void OpenFileUsingService(const QString &path, const QString &service);

private:
    QMap<QString, QStringList> services;
    QString configFilePath = "services.ini";

    void loadServices();
    void saveServices();
    bool isServiceRunning(const QString &service);
};
