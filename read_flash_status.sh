#!/bin/sh

if [ $# -lt 2 ]; then
  echo "Usage: $0 <serial port> (read|write)"
  exit 1
fi
if [ ! -e $1 ]; then
  echo "Serial port $1 does not exist"
  exit 1
fi
if [ $2 != "read" -a $2 != "write" ]; then
  echo "Usage: $0 <serial port> read|write"
  exit 1
fi
if [ $2 == "read" ]; then
  esptool.py --port $1 read_flash_status --bytes 3
  exit 0
fi
if [$2 == "write" ]; then
  esptool.py --port $1 write_flash_status --bytes 2 --non-volatile 0
  exit 0
fi