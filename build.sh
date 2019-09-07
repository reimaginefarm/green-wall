#!/bin/sh
(git pull && cd code && platformio run --target upload)
