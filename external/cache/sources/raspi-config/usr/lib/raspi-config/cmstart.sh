#!/bin/sh
if raspi-config nonint is_kms ; then
    if ps ax | grep -v grep | grep -q openbox ; then
        exec xcompmgr -aR
    fi
fi
