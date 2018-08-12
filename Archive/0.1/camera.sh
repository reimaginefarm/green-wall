#!/bin/bash

DATE=$(date +"%Y-%m-%d_%H%M")

#cheese & disown cheese
#sleep 30
#killall -9 cheese
#sleep 10

fswebcam -r 2304x1536 -S 60 --no-banner /home/greenwall/camera/$DATE.jpg
