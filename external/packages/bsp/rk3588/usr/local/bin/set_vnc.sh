#!/bin/bash

[[ -d ~/.vnc ]] && rm -rf ~/.vnc

vncserver
vncserver -kill :1
mv ~/.vnc/xstartup ~/.vnc/xstartup.bak

cat <<-EOF >  \
~/.vnc/xstartup
#!/bin/bash
xrdb $HOME/.Xresources
startxfce4 &
EOF

chmod +x ~/.vnc/xstartup
vncserver

sync
