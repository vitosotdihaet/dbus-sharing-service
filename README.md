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
- `BUILD_SRS` - builds the Sharing Register Service *(ON by default)*
- `BUILD_SS` - builds the Sharing Service dynamic library *(ON by default)*
- `BUILD_EXAMPLES` - builds examples *(OFF by default)*


# Sharing Register Service
Sharing Register Service is a DBus service, with path of `com.system.sharing`, which purpose is to store other DBus service paths for sharing files.

Instead of remembering all the DBus services file paths for each file type, you can register a service path with supported formats in the Sharing Register Service

Now, on opening the file with `com.system.sharing`, one of the registered services that support this file format would open it

Sharing Register Service has three methods:
- `RegisterService`, that accepts a DBus object path *(string)* to be registered and its supported file formats *(array of strings)*
- `OpenFileUsingService`, that accepts an absoulte path to the file *(string)* and a DBus object path *(string)*. The method `OpenFile` of the provided object is run with the file path as its first argument
- `OpenFile`, that accepts an absolute path to the file *(string)*. The method `OpenFile` of one of the provided objects, that supports the file's format is run with the file path as its first argument

## How to run
```bash
./sharingRegisterService/srs
```

To exit, simply press `Ctrl+C`

## How to use
Registering a service:
```bash
gdbus call -e -d com.system.sharing -o / -m com.system.sharing.RegisterService "your.dbus.service" "[\"supported\", \"extensions\"]"
```

Opening a file:
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

For working with files in a working directory, instead of writing the whole absolute path just start your filepath with `` `pwd` ``, e. g. instead of `"/home/user/Documents/important/file.mp3"` (assuming you are in the `Documents folder`), use ``"`pwd`/important/file.mp3"``

# Sharing Service
Sharing Service is a dynamic C++ library that can be used for an easy development of services

Class `SharingService` is the only thing added with this library. On construction, a new DBus object is created with a single method - `OpenFile`

The constructor of the `SharingService` accepts three arguments: a DBus object path, the supported formats of the service and a function, accepting `QString` and returning `void` to be run when method `OpenFile` is invoked

## How to use (`cmake`)
After building with `BUILD_LIBSS`, you need to link the library and include the header file to your target program

Set up your `main.cpp`:
```cpp
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

Next, update your cmake building script - add linking to `libss.so` and include the header:
- Copy the shared library `build/sharingService/libss.so` and put it into your `lib` folder
- Copy the header `sharingService/include/sharingService.hpp` to your `include` folder
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core DBus)
qt6_wrap_cpp(MOC_SOURCES include/sharingService.hpp)
link_directories(lib)
add_executable(main src/main.cpp ${MOC_SOURCES})
target_link_libraries(main PRIVATE Qt6::Core Qt6::DBus ss)
```

To exit, press `Ctrl+C`

## Accessing the service directly
```bash
gdbus call -e -d my.audio.service -o / -m my.audio.service.OpenFile "`pwd`/Documents/sound.mp3"
```

# Examples
There are examples in the examples folder, to build them, go to the `build` folder and run:

```bash
cmake -DBUILD_EXAMPLES=ON ..
make
```

## SharingServiceExample
Shows a basic setup for using the Sharing Service ***(note that you need to run Sharing Register Service as a separate process for it to work)***

```bash
./examples/example1/SharingServiceExample
```