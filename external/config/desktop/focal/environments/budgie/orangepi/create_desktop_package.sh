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

#generate wallpaper list for background changer
mkdir -p "${destination}"/usr/share/cinnamon-background-properties
cat <<EOF > "${destination}"/usr/share/cinnamon-background-properties/orangepi.xml
<?xml version="1.0"?>
<!DOCTYPE wallpapers SYSTEM "cinnamon-wp-list.dtd">
<wallpapers>
  <wallpaper deleted="false">
    <name>OrangePi black-pyscho</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-black-psycho.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi bluie-circle</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-blue-circle.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi blue-monday</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-blue-monday.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi blue-penguin</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-blue-penguin.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi gray-resultado</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-gray.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi green-penguin</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-green-penguin.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi green-retro</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-green-retro.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi green-wall-penguin</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-green-wall-penguin.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi 4k-neglated</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-neglated.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi neon-gray-penguin</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-neon-gray-penguin.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi plastic-love</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-plastic-love.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi purple-penguine</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-purple-penguine.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
    <wallpaper deleted="false">
    <name>OrangePi purplepunk-resultado</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-purplepunk.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi red-penguin-dark</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-red-penguin-dark.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi red-penguin</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-4k-red-penguin.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi light</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi18-Dre0x-Minum-light-3840x2160.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi dark</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi03-Dre0x-Minum-dark-3840x2160.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi uc</name>
    <filename>/usr/share/backgrounds/orangepi/orangepi-full-under-construction-3840-2160.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
  <wallpaper deleted="false">
    <name>OrangePi clear</name>
    <filename>/usr/share/backgrounds/orangepi/OrangePi-clear-rounded-bakcground-3840-2160.jpg</filename>
    <options>zoom</options>
    <pcolor>#ffffff</pcolor>
    <scolor>#000000</scolor>
  </wallpaper>
</wallpapers>
EOF
