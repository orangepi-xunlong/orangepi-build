#!/bin/sh
# pgload, v2.1 2011/09/23
module="pgdrv"
#mode="664"

# check whrther the kernel is valid or not
#version1=`uname -r | cut -d '.' -f 1`
#version2=`uname -r | cut -d '.' -f 2`
#version3=`uname -r | cut -d '.' -f 3`
#version3=${version3%%-*}
#if [ "$version1" != "2" ] || [ "$version2" != "6" ] ; then
#        echo "    Your version of kernel is `uname -r`"
#        echo "    The kernel is too outdated to be used."
#        echo "    Please update the version to 2.6.16 or later."
#        exit 1
#elif test $version3 -lt 11 ; then
#        echo "    Your version of kernel is `uname -r`"
#        echo "    The kernel is outdated."
#        echo "    Please update the version to 2.6.11 or later."
#fi

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
check=`lsmod | grep r8169`
if [ "$check" != "" ]; then
        echo "rmmod r8169"
        /sbin/rmmod r8169
fi

check=`lsmod | grep r8168`
if [ "$check" != "" ]; then
        echo "rmmod r8168"
        /sbin/rmmod r8168
fi

check=`lsmod | grep r8125`
if [ "$check" != "" ]; then
        echo "rmmod r8125"
        /sbin/rmmod r8125
fi

check=`lsmod | grep r8101`
if [ "$check" != "" ]; then
        echo "rmmod r8101"
        /sbin/rmmod r8101
fi

check=`lsmod | grep $module`
if [ "$check" != "" ]; then
        echo "$module has existed."
        echo "$check"
        exit 1
else
        if test -e ./$module.ko ; then
                echo "insmod $module.ko $*"
                /sbin/insmod ./$module.ko $* || exit 1
        else
                echo "make clean all"
                make clean all 1>/dev/null
                if test -e ./$module.ko ; then
                        echo "insmod $module.ko $*"
                        /sbin/insmod ./$module.ko $* || exit 1
                else
                        echo "$module.ko doesn't exist."
                        exit 1
                fi
        fi
fi

exit 0
