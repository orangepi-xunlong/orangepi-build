#!/bin/bash -x

export DISPLAY=${DISPLAY:-:0}

function prepare_env() {
    # Try to figure out XAUTHORITY and DISPLAY
    for pid in $(pgrep X 2>/dev/null || \
        ls /proc|grep -ow "[0-9]*"|sort -rn); do
        PROC_DIR=/proc/$pid

        # Filter out non-X processes
        readlink $PROC_DIR/exe|grep -qwE "X$|Xorg$" || continue

        # Parse auth file and display from cmd args
        export XAUTHORITY=$(cat $PROC_DIR/cmdline|tr '\0' '\n'| \
            grep -w "\-auth" -A 1|tail -1)
        export DISPLAY=$(cat $PROC_DIR/cmdline|tr '\0' '\n'| \
            grep -w "^:.*" || echo ":0")

        logger -t $0 "Found auth: $XAUTHORITY for dpy: $DISPLAY"
        return
    done
}

function xrandr_wrapper() {
    xrandr --screen ${SCREEN:-0} $@
}

if ! xdpyinfo &>/dev/null; then
    # Try to setup env
    prepare_env

    if ! xdpyinfo &>/dev/null; then
        # Try to switch to an authorized user
        for XUSER in root $(users);do
            sudo -u $XUSER xdpyinfo &>/dev/null || continue

            logger -t $0 "Switch to user: $XUSER"
            sudo -u $XUSER $0; exit 0
        done

        logger -t $0 "Unable to contact Xserver!"
        exit 0
    fi
fi

TMP=$(mktemp)

SCREENS=$(xdpyinfo|grep screens|cut -d':' -f2)
for SCREEN in $(seq 0 ${SCREENS:-0}); do
    # Get monitors and current mode info
    xrandr_wrapper 2>&1|grep -oE "^.*connected|^.*\*"|tee $TMP

    # Result:
    # eDP-1 connected
    #   1536x2048     59.99*
    # DP-1 disconnected
    # HDMI-1 connected
    #   1920x1080     60.00*
    #
    # "*" means valid current mode.

    # Make sure every connected monitors been enabled with a valid mode.
    for MONITOR in $(grep -w connected $TMP|cut -d' ' -f1); do
        # Find monitors without a valid current mode
        if ! grep -w $MONITOR $TMP -A 1|grep "\*"; then
            # Ether disabled or wrongly configured
            xrandr_wrapper --output $MONITOR --auto

            logger -t $0 "Output $MONITOR enabled."
        fi
    done
done

rm -rf $TMP

exit 0
