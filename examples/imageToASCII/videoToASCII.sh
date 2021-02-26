#!/bin/bash
input_file="./input"
rm  -f $input_file/*
#write out the images
ffmpeg -i $1 -vf fps=24 $input_file/out%d.png
#write out the audio
ffmpeg -y -i $1 audio.wav

if [ $? -eq 1 ]
then
	echo "enter a video!"
	exit 1
fi

input_images=$(ls  --sort=version $input_file)
size=$(ls $input_file | wc -l)
output_file="./output"
let "i=0"
rm  -f $output_file/*
for file_name in $input_images; do
	./imageToASCII $input_file"/"$file_name 1 $output_file/"out$i"	

	let "i=i+1"
	let "left=size-i"
	echo "wrote $file_name, only $left images to go"
done
ffmpeg -y -i audio.wav -r 24 -f image2 -start_number 1 -i "$output_file/out%d.png"  -preset slow -vcodec libx264 -vf eq=brightness=0.06:saturation=2 -crf 18  -pix_fmt yuv420p out.mp4 
rm audio.wav
