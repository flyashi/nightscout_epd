#!/bin/bash

fw_file="autoupdate.h"

cur_version=$(grep "const int FW_VERSION" ${fw_file} | grep -o -E '[0-9]{10}')
echo "cur_version=${cur_version}[end]"

datepart=${cur_version:0:8}
echo $datepart

curdate=$(date +%Y%m%d)

if [ $datepart != $curdate ]; then
	newver=${curdate}01
	echo "New date, setting to ${newver}"
else
	echo "cur version being incremented: $cur_version"
	newver=$(( cur_version + 1 ))
	echo "Same date, incremented to $newver"
fi

echo "Replacing $cur_version with $newver ..."
sed -i "s/const int FW_VERSION = ${cur_version};/const int FW_VERSION = ${newver};/g" ${fw_file}
echo "Done."