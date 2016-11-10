#!/bin/bash
# handle_dir
Nlpir_Seg_Home=/home/yanfa/yanqiwei/proj/NLPIR/ICTCLAS2014/src/sample/c_seg

tools_home=/home/yanfa/yanqiwei/proj

input_file=$1

file_name=${input_file##*/}

res_dir=$2

cat $1 |python $tools_home/util_tools/win2unix/remove_speChar.py > $1.ok

$Nlpir_Seg_Home/nlpir_seg_file $1.ok $1_output.txt $3 $4 $5 $6

if test "$3" == "2"
    then
    	python get_pos_result.py $1_output.txt $1_output.txt.ok
	mv $1_output.txt.ok $1_output.txt
fi

if test "$res_dir" != "file"
    then
        cp $1_output.txt $res_dir/$file_name
else
    cp $1_output.txt $input_file.final_res.txt
fi

rm -fr $1.ok
rm -fr $1_output.txt
