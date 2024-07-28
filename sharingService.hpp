#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>


class SharingService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.system.sharing")

public slots:
    void RegisterService(const QString &name, const QStringList &supportedFormats);
    void OpenFile(const QString &path);
    void OpenFileUsingService(const QString &path, const QString &service);

private:
    QMap<QString, QStringList> services;
};
