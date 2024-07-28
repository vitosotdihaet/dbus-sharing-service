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
### How to use [TODO]


### Accessing after running
Directly
```bash
gdbus call -e -d your.service.name -o / -m your.service.name.OpenFile "/your/absolute/file.path"
```

