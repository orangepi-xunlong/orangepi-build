# install lightdm greeter
cp -R "${EXTER}"/packages/blobs/desktop/lightdm "${destination}"/etc/orangepi

# install default desktop settings
mkdir -p "${destination}"/etc/skel
cp -R "${EXTER}"/packages/blobs/desktop/skel/. "${destination}"/etc/skel

#install cinnamon desktop bar icons
mkdir -p "${destination}"/usr/share/icons/orangepi
cp "${EXTER}"/packages/blobs/desktop/desktop-icons/*.png "${destination}"/usr/share/icons/orangepi

# install wallpapers
mkdir -p "${destination}"/usr/share/backgrounds/orangepi/
cp "${EXTER}"/packages/blobs/desktop/desktop-wallpapers/*.jpg "${destination}"/usr/share/backgrounds/orangepi

# install wallpapers
mkdir -p "${destination}"/usr/share/backgrounds/orangepi-lightdm/
cp "${EXTER}"/packages/blobs/desktop/lightdm-wallpapers/*.jpg "${destination}"/usr/share/backgrounds/orangepi-lightdm

# install logo for login screen
mkdir -p "${destination}"/usr/share/pixmaps/orangepi
cp "${EXTER}"/packages/blobs/desktop/icons/orangepi.png "${destination}"/usr/share/pixmaps/orangepi
