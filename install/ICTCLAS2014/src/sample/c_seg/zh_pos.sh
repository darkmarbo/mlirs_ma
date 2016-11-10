#!/bin/bash
Nlpir_Seg_Home=/home/yanfa/yanqiwei/proj/NLPIR/ICTCLAS2014/src/sample/c_seg

PWD=`pwd`

input_file=$1

file_name=${input_file##*/}
suffix=${input_file##*.}

res_file=$1.zh-pos_res.zip

echo $*

POStagged=$3
file_lang=$5
file_encoding=$7
userdict_path=$9

echo "inputfile "$file_name
#echo $suffix
echo "POStagged "$POStagged
echo "file_lang "$file_lang
echo "file_encoding "$file_encoding
echo "userdict_path "$userdict_path


cd /data/web/script/yanqiwei/proj/NLPIR/ICTCLAS2014/src/sample/c_seg

if test "$suffix" == "rar"
    then
        echo "请上传zip文件夹"
        exit 11
elif test "$suffix" != "zip"
    then
	sh $Nlpir_Seg_Home/handle_file_pos.sh $input_file "file" $POStagged $file_lang $file_encoding $userdict_path
        exit 1
fi

mkdir $PWD/data
cp $1 $PWD/data/
unzip $PWD/data/$file_name -d $PWD/data/
rm -fr $PWD/data/$file_name

mkdir $PWD/data_res

sh $Nlpir_Seg_Home/handle_dir.sh $PWD/data $PWD/data_res $POStagged $file_lang $file_encoding $userdict_path

cd $PWD
zip -r $res_file data_res

rm -fr $PWD/data
rm -fr $PWD/data_res
