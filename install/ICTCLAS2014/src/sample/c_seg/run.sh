#!/bin/sh


if(($#<2))
then 
	echo "usage: $0 input_file output_file"
	exit 0
fi

seg_dir=/home/szm/cd/ICTCLAS2014/src/sample/c_seg
export LD_LIBRARY_PATH=$seg_dir:$LD_LIBRARY_PATH
./nlpir_seg_file $1 $2 1 simplified UTF8 userdict_path
