#!/bin/bash

arduino-cli compile -e --fqbn=esp32:esp32:esp32:CPUFreq=240,UploadSpeed=921600 --build-property build.partitions=min_spiffs --build-property upload.maximum_size=1966080
