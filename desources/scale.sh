#!/bin/bash

size=128
cat source_images.txt | sed "s#\(.*\)\\.png#echo \"scaling: \1.png --> \1_small.png\"; convert \1.png -resize ${size}x${size} \1_small.png#g" > temp.sh; . ./temp.sh; rm -f temp.sh


