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

You can also use some `cmake` flags to customize your building process:
- `BUILD_SRS` - builds the sharing register service *(ON by default)*
- `BUILD_SS` - builds the sharing service dynamic library *(ON by default)*
- `BUILD_EXAMPLES` - builds examples on how to use the sharing service dynamic library *(OFF by default, works only if `BUILD_SS` is ON)*

# How to run
```bash
./srs # stands for Sharing Register Service
```

On running, a DBus process is created, named `com.system.sharing`, with three methods:
- `RegisterService`, that accepts a name *(string)* of a DBus service to be registered and its supported file formats *(array of strings)*
- `OpenFileUsingService`, that accepts an absoulte path to the file *(string)* and a service name *(string)*. The method `OpenFile` of the provided service is run on the file path
- `OpenFile`, that accepts an absolute path to the file *(string)*. The method `OpenFile` of one of the provided services, that supports the file's format is run on the file path

# How to use
## From a terminal
Registering a service:
```bash
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.RegisterService "your.dbus.service" "[\"supported\", \"extensions\"]"
```

Opening a file *(with one of the random registered services, that support files with this extension)*:
```bash
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.OpenFile "/your/absolute/file.path"
```

Opening a file with a particular service:
```bash
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.OpenFileUsingService "/your/absolute/file.path" "your.dbus.service"
```

### Examples
```bash
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.RegisterService "my.epic.service" "[\"txt\", \"mp4\"]"
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.RegisterService "my.text.service" "[\"txt\"]"
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.OpenFile "/home/user/Documents/favourite-cat-names.txt"
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.OpenFileUsingService "/home/user/Documents/favourite-cat-names.txt" "my.epic.service"
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.OpenFile "/home/user/Documents/video.mp4"
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

### Examples
There are examples in the examples folder, to build them, go to the `build` folder and run:
```bash
cmake -DBUILD_EXAMPLES=ON ..
make
# now you can run any example in the examples folder, for example:
./examples/example1/SharingServiceExample # note that you need to run build/srs to connect to it
```
