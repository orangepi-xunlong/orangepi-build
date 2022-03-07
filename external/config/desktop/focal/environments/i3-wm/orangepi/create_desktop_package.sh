# install lightdm greeter
cp -R "${EXTER}"/packages/blobs/desktop/lightdm "${destination}"/etc/orangepi

# install default desktop settings
mkdir -p "${destination}"/etc/skel
cp -R "${EXTER}"/packages/blobs/desktop/skel/. "${destination}"/etc/skel

# install logo for login screen
mkdir -p "${destination}"/usr/share/pixmaps/orangepi
cp "${EXTER}"/packages/blobs/desktop/icons/orangepi.png "${destination}"/usr/share/pixmaps/orangepi

# install wallpapers
mkdir -p "${destination}"/usr/share/backgrounds/orangepi/
cp "${EXTER}"/packages/blobs/desktop/wallpapers/orangepi*.jpg "${destination}"/usr/share/backgrounds/orangepi/
