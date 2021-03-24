#!/bin/bash
if [ -z "$1" ] ;
then
	echo "please enter a file that contains all your images"
	exit 1 
fi
output=$2
if [ -z "$output"];
then
	output="."
fi

ffmpeg -y -r 60 -f image2 -start_number 1 -i "$1/out%d.png"  -vcodec libx264  -crf 24 -vf "fps=60,format=yuv420p" "$output/out.mp4"

