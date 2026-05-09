## Requirements

### MacosX
* Install docker-desktop
* Enable "Use Rosetta for x86_64/amd64 emulation on Apple Silicon" in docker desktop setting

### Windows

## Build

```shell
# build docker image
./docker-build.sh --build-image

# build orangepi os image
./docker-build.sh
```

## Output folders

* Debug info - orangepi-output/debug
* Image - orangepi-output/images (ex. orangepi-build/orangepi-output/images/Orangepi4pro_1.0.8_debian_bullseye_desktop_xfce_linux6.6.98/Orangepi4pro_1.0.8_debian_bullseye_desktop_xfce_linux6.6.98.img)
* deb packages - orangepi-output/debs
