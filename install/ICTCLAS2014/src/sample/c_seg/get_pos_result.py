#coding=utf-8
#! python

import sys;
import os;
import re;

posDict = {}


lineNum = 0
dictPath = "ICTPOS-PK2001.txt"
for line in open(dictPath,'r').xreadlines():
	contents = line[:-1].strip().split('\t')
	lineNum += 1
	if len(contents)<2:
		print "Error 1\t"+str(lineNum)+"\t"+line[:-1]
		continue

	posDict[contents[0].strip()] = contents[1].strip()
	

fpw = open(sys.argv[2],'w')

new_line = ""
lineNum = 0
for line in open(sys.argv[1],'r').xreadlines():
	line = line[:-1].strip()
	lineNum += 1
	if not line:
		continue
        line,num = re.subn("[ ]+"," ",line)
        word_py_arrs = line.strip().split(" ")
        new_line = ""
        for index_i in range(0, len(word_py_arrs)):
                word_py_str = word_py_arrs[index_i];
                word_py_arr = word_py_str.split("/")
                word = word_py_arr[0]
                pos = "nx"
                if len(word_py_arr) < 2:
                        print "Error 2\t"+str(lineNum)+"\t"+line
                        
                        
                else:
                        pos = word_py_arr[1]
                        if pos in posDict:
                                pos = posDict[pos]
                new_line += word+"/"+pos+" "

        fpw.write(new_line[:-1]+"\n")

fpw.close()
