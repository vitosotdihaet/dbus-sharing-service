# Prerequisites
Make sure Qt6 is installed on your machine, if not, please visit [this page](https://www.qt.io/download-qt-installer-oss?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)

# How to run
```bash
git clone https://github.com/vitosotdihaet/dbus-sharing-service.git
cd dbus-sharing-service
mkdir build && cd build
cmake ..
make
./SharingService
```

# How to use
## From a terminal
Registering a service:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "your.dbus.service" "[\"supported\", \"extensions\"]"
```

Opening a file *(with one of the random registered services, that support files with this extension)*:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/your/absolute/file/path"
```

Opening a file with a particular service:
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFileUsingService "/your/absolute/file/path" "your.dbus.service"
```

### Examples
```bash
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "my.epic.service" "[\"txt\", \"mp4\"]"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.RegisterService "my.text.service" "[\"txt\"]"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/home/user/Documents/favourite-cat-names.txt"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFileUsingService "/home/user/Documents/favourite-cat-names.txt" "my.epic.service"
gdbus call --session --dest com.system.sharing --object-path / --method com.system.sharing.OpenFile "/home/user/Documents/video.mp4"
```

## As a C++ library [TODO]