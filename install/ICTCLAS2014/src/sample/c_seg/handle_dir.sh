#!/bin/bash
# handle_dir
Nlpir_Seg_Home=/home/yanfa/yanqiwei/proj/NLPIR/ICTCLAS2014/src/sample/c_seg

input_dir=$1
res_dir=$2
#POStagged=$3
#file_lang=$4
#file_encoding=$5
#userdict_path=$6

#echo $input_dir

for FILES in `ls $input_dir`
do
    if [ -d $input_dir"/"$FILES ]
        then
            echo "handling dir $input_dir/$FILES"
            mkdir $2/$FILES
            sh $Nlpir_Seg_Home/handle_dir.sh $input_dir/$FILES $2/$FILES $3 $4 $5 $6
    else
        #echo "handling file $input_dir/$FILES"
        sh $Nlpir_Seg_Home/handle_file_pos.sh $input_dir/$FILES $2 $3 $4 $5 $6
    fi

done
