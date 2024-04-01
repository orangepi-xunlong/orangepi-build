#!/bin/bash

#bl2 file size 41K, bl21 file size 3K (file size not equal runtime size)
#total 44K
#after encrypt process, bl2 add 4K header, cut off 4K tail

#bl30 limit 41K
#bl301 limit 12K
#bl2 limit 41K
#bl21 limit 3K, but encrypt tool need 48K bl2.bin, so fix to 7168byte.

#$7:name flag
if [ "$7" = "bl30" ]; then
	declare blx_bin_limit=40960
	declare blx01_bin_limit=13312
elif [ "$7" = "bl2" ]; then
	declare blx_bin_limit=57344
	declare blx01_bin_limit=4096
else
	echo "blx_fix name flag not supported!"
	exit 1
fi

# blx_size: blx.bin size, zero_size: fill with zeros
declare -i blx_size=`du -b $1 | awk '{print int($1)}'`
declare -i zero_size=$blx_bin_limit-$blx_size
dd if=/dev/zero of=$2 bs=1 count=$zero_size
cat $1 $2 > $3
rm $2

declare -i blx01_size=`du -b $4 | awk '{print int($1)}'`
declare -i zero_size_01=$blx01_bin_limit-$blx01_size
dd if=/dev/zero of=$2 bs=1 count=$zero_size_01
cat $4 $2 > $5

cat $3 $5 > $6

rm $2

exit 0
