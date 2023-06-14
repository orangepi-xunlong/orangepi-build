#!/bin/bash

source /etc/orangepi-release

es8388_card=$(aplay -l | grep "es8388" | cut -d ':' -f 1 | cut -d ' ' -f 2)
jack_num=$(tinymix -D ${es8388_card} | grep "Headphone Jack" | cut -c1-2)

[[ ${BOARD} != orangepi900 ]] && exit

if [[ $(tinymix -D ${es8388_card} $jack_num | cut -d ":" -f 2) == *On ]]; then

	#for playback
	tinymix -D ${es8388_card} 25 2
	tinymix -D ${es8388_card} 27 2

	#for capture
	tinymix -D ${es8388_card} 35 0
	tinymix -D ${es8388_card} 36 0
	tinymix -D ${es8388_card} 37 0

else

	#for playback
	tinymix -D ${es8388_card} 25 0
	tinymix -D ${es8388_card} 27 0

	#for capture
	tinymix -D ${es8388_card} 35 1
	tinymix -D ${es8388_card} 36 1
	tinymix -D ${es8388_card} 37 1

fi

tinymix -D ${es8388_card} 3 4
tinymix -D ${es8388_card} 4 2
tinymix -D ${es8388_card} 14 192
tinymix -D ${es8388_card} 16 4
tinymix -D ${es8388_card} 17 4
tinymix -D ${es8388_card} 23 30
tinymix -D ${es8388_card} 24 30
