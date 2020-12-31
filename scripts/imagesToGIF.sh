#!/bin/bash

./imagesToVideo.sh $1 $2
if [ "$?" -eq "1" ]
then
	exit 
fi

vid_path=$2
if [ -z "$output" ]
then
	vid_path="."
fi

ffmpeg  -t 4 -i $vid_path/out.mp4 -vf "fps=24,scale=320:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 $vid_path/out.gif 
rm $vid_path/out.mp4
