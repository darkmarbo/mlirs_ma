#coding=utf-8
#! python

import sys;
import re;
import os;
import codecs;
import jieba;
import jieba.posseg as pseg;


def handle_file(input_path, output_path):

        fpw = open(output_path, 'w')
        lineNum = 0
        for line in open(input_path).xreadlines():
                lineNum += 1
                #print str(lineNum)
                try:
                        new_line = line[:-1].strip().decode('utf-8')
                except UnicodeDecodeError, e:
                        print e
                        continue

                if lineNum == 1:
                        new_line = new_line.lstrip(unicode(codecs.BOM_UTF8, "utf8"))
                
                seg_list = jieba.cut(new_line, cut_all=False)

                try:
                        #fpw.write((" ".join(seg_list)+"\n").encode('utf-8'))
                        fpw.write((" ".join(seg_list)+"\n").encode('utf-8'))
                except UnicodeEncodeError, e:
                        print e
                        continue
                
        fpw.close()


def handle_dir(input_path, output_path):
        if not os.path.exists(input_path):
                print "Error, dir "+input_path+" not exists!"
                return
        
        if not os.path.isdir(input_path):
                print "Error, "+input_path+" is not directory!"
                return
        
        if not os.path.exists(output_path):
                os.mkdir(output_path)

        file_list = os.listdir(input_path)
        for file_name in file_list:
                if os.path.isdir(input_path+"/"+file_name):
                        handle_file(input_path+"/"+file_name, output_path+"/"+file_name)
                else:
                        handle_file(input_path+"/"+file_name, output_path+"/"+file_name)


def handle(input_path, output_path):
        if os.path.isdir(input_path):
                handle_dir(input_path, output_path)
        else:
                handle_file(input_path, output_path)
        

if __name__=='__main__':
        if len(sys.argv) < 3:
                print "Usage: python Seg_jieba_Dir.py input_path output_path"
                exit(-1);
                
        input_path = sys.argv[1];
        output_path = sys.argv[2];
        
        handle(input_path, output_path)
        
