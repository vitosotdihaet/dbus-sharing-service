#include "sharingService.hpp"

#include <QCoreApplication>
#include <QDBusConnection>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService sharingService(nullptr);

    return app.exec();
}
