#include "sharingRegisterService.hpp"

#include <QCoreApplication>
#include <QDBusConnection>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingRegisterService sharingService;

    return app.exec();
}
