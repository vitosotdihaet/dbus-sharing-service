#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDBusVirtualObject>
#include <functional>


class SharingService : public QDBusVirtualObject {
    Q_OBJECT

public:
    explicit SharingService(const QString &serviceName, const QStringList &supportedFormats, std::function<void(const QString &)> onOpenFile, QObject *parent = nullptr);
    bool start();

    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection) override;
    QString introspect(const QString &path) const override;

public slots:
    void OpenFile(const QString &path);

private:
    QString serviceName;
    QStringList supportedFormats;
    std::function<void(const QString &)> onOpenFile;
    bool registerService();
    bool registerObject();
    bool addToSharingRegisterService();
};
