#!/bin/bash

for LANG in *
do
    if [ ! -d "$LANG" ]; then
        continue
    fi

	cd "$LANG" || continue

	# check for run script
	if [ ! -f run.sh ]; then
		cd ..
		continue
	fi

    # check for skip
    if [ -f skip ]; then
        echo "Skipping $LANG due to presence of skip file"
        cd ..
        continue
    fi

    echo
    echo "Running $LANG Benchmarks"
    # run install script
    ./run.sh
    cd ..
done