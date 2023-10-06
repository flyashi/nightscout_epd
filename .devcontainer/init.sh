#!/bin/bash
mkdir ~/bin
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/bin sh
sudo mv ~/bin/arduino-cli /usr/local/bin
rmdir ~/bin
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core install esp32:esp32
curl -sL https://bootstrap.pypa.io/get-pip.py | python3
python3 -m pip install pyserial
sudo apk add gcompat