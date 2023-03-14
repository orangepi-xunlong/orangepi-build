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
cp "${EXTER}"/packages/blobs/desktop/desktop-wallpapers/*.png "${destination}"/usr/share/backgrounds/orangepi

# install wallpapers
mkdir -p "${destination}"/usr/share/backgrounds/orangepi-lightdm/
cp "${EXTER}"/packages/blobs/desktop/lightdm-wallpapers/*.png "${destination}"/usr/share/backgrounds/orangepi-lightdm

# install logo for login screen
mkdir -p "${destination}"/usr/share/pixmaps/orangepi
cp "${EXTER}"/packages/blobs/desktop/icons/orangepi.png "${destination}"/usr/share/pixmaps/orangepi

# set default wallpaper
#echo "
#dbus-send --session --dest=org.kde.plasmashell --type=method_call /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string:
#var Desktops = desktops();
#for (i=0;i<Desktops.length;i++) {
#        d = Desktops[i];
#        d.wallpaperPlugin = \"org.kde.image\";
#        d.currentConfigGroup = Array(\"Wallpaper\",
#                                    \"org.kde.image\",
#                                    \"General\");
#        d.writeConfig(\"Image\", \"file:///usr/share/backgrounds/orangepi/orangepi03-Dre0x-Minum-dark-3840x2160.jpg\");
#}'" > "${destination}"/usr/share/backgrounds/orangepi/set-orangepi-wallpaper.sh
