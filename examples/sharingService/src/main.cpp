#include "sharingService.hpp"

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QtGlobal>

void onOpenFile(const QString &path) {
  qInfo() << "File" << path << "is opened";
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  QStringList supportedFormats = {"mp3"};
  SharingService service("my.audio.service", supportedFormats, onOpenFile);

  if (!service.start()) {
    qInfo() << "Failed to start SharingService";
    return 1;
  }

  return app.exec();
}
