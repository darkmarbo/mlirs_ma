#!/bin/sh -x 

rm -rf data/lm/*
rm -rf data/seg/*

### 分词
#seg_dir=/home/srilm_test/word_seg
seg_dir=/home/srilm_test/ICTCLAS2014/src/sample/c_seg
ls -1 data/text|while read line
do
	cp -r data/text/$line $seg_dir 
	cd $seg_dir && ./run.sh $line $line.seg && cd - 
	mv $seg_dir/$line.seg data/seg 
done

## 选词典
cat data/seg/* > seg.all
python count.py seg.all seg.all.count  
sort -n -r seg.all.count|head -100000 > seg.all.count.sort
awk -F"\t" '{print $2}'  seg.all.count.sort > seg.all.count.sort.awk
mv seg.all.count.sort.awk data/vocab.txt
rm -rf seg.all*

### 训练文本
cat data/seg/* |gzip > data/seg_all.gz

vocab=data/vocab.txt
full_corpus=data/seg_all.gz
lm_out=data/lm/out_lm_4.arpa
trigram_lm=data/lm/out_lm_3.arpa

### 3-gram 
ngram-count -order 3  -kndiscount -interpolate -unk -map-unk "<UNK>" -limit-vocab -vocab $vocab -text $full_corpus -lm $trigram_lm || exit 1

### 3-gram prune
prune_thresh_small=0.0000003
trigram_pruned_small=data/lm/out_lm_3_prune.arpa
ngram -prune $prune_thresh_small -lm $trigram_lm -write-lm $trigram_pruned_small || exit 1

#### 4-gram
ngram-count -order 4  -kndiscount -interpolate -unk -map-unk "<UNK>" -limit-vocab -vocab $vocab -text $full_corpus -lm $lm_out || exit 1

#cat data/lm/test.arpa | gzip > data/lm/test.arpa.gz
