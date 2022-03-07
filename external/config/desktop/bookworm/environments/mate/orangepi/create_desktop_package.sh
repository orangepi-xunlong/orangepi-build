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

mkdir -p "${destination}"/usr/share/mate-background-properties
cat <<-EOF > "${destination}"/usr/share/mate-background-properties/orangepi.xml
<?xml version="1.0"?>
<!DOCTYPE wallpapers SYSTEM "gnome-wp-list.dtd">
<wallpapers>
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
</wallpapers>
EOF

mkdir -p "${destination}"/usr/share/glib-2.0/schemas
cat <<-EOF > "${destination}"/usr/share/glib-2.0/schemas/org.gnome.desktop.background.gschema.override
[org.gnome.desktop.background]
picture-uri='file:///usr/share/backgrounds/orangepi/orangepi03-Dre0x-Minum-dark-3840x2160.jpg'
show-desktop-icons=true
EOF
