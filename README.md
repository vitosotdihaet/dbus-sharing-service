# Prerequisites
Make sure Qt6 is installed on your machine, if not, please follow [this tutorial](https://doc.qt.io/qt-6/get-and-install-qt.html)

# How to build
```bash
git clone https://github.com/vitosotdihaet/dbus-sharing-service.git
cd dbus-sharing-service
mkdir build && cd build
cmake ..
make
```

# How to run
```bash
./srs # stands for Sharing Register Service
```

# How to use
## From a terminal
Registering a service:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "your.dbus.service" "[\"supported\", \"extensions\"]"
```

Opening a file *(with one of the random registered services, that support files with this extension)*:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/your/absolute/file.path"
```

Opening a file with a particular service:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFileUsingService "/your/absolute/file.path" "your.dbus.service"
```

### Examples
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "my.epic.service" "[\"txt\", \"mp4\"]"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "my.text.service" "[\"txt\"]"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/home/user/Documents/favourite-cat-names.txt"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFileUsingService "/home/user/Documents/favourite-cat-names.txt" "my.epic.service"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/home/user/Documents/video.mp4"
```

## As a C++ dynamic library
### How to use (cmake)
After building SharingService, you need to add the library and the header file to your project
- Copy the shared library `libss.so` and put it into your `library`
- Copy the header `sharingService/include/sharingService.hpp` to your `include` folder

Set up your `main.cpp`:
```
#include "sharingService.hpp"

#include <QtGlobal>
#include <QCoreApplication>
#include <QString>
#include <QStringList>


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
```

The `onOpenFile` function, that is passed to the constructor must be of type `void` and accept a single parameter `const QString &`

Next, update your cmake building script: add linking to `libss.so` *(create moc sources, link directories, link the library to your target)*:
```cmake
qt6_wrap_cpp(MOC_SOURCES ${CMAKE_SOURCE_DIR}/include/sharingService.hpp)
link_directories(${CMAKE_SOURCE_DIR}/lib)
add_executable(main src/main.cpp ${MOC_SOURCES})
target_link_libraries(main PRIVATE Qt6::DBus ss)
```

### Accessing directly after running
```bash
gdbus call -e -d your.service.name -o / -m your.service.name.OpenFile "/your/absolute/file.path"
```
