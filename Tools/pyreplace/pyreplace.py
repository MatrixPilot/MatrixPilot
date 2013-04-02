'''
Created on 31 mar 2013

@author: Matt
'''

import re, os, sys, shutil
from time import sleep
import re

class searchstr(object):
    def __init__(self, find, replacewith):
        self.find = find
        self.replacewith = replacewith


class replacer(object):
    def __init__(self, rootdir, opts, searchstrings, predelim, postdelim):
        self.rootdir = rootdir
        self.opts = opts
        self.sstr = searchstrings
        self.predelim = predelim
        self.postdelim = postdelim
        self.fileList = []
        
    def find_files(self):
        self.fileList = []
        
        print("search directories are")
        print(opts.directories)

        for directory in self.opts.directories:
            path = os.path.join(self.rootdir, directory)

            fileSize = 0
            folderCount = 0    

            for root, subFolders, files in os.walk(path):
                folderCount += len(subFolders)
                for filename in files:
                    f = os.path.join(root,filename)
                    fileSize = fileSize + os.path.getsize(f)
                    #print(f)
                    splt = filename.split(".")
                    if(len(splt) == 2):
                        if(splt[1] == "c"):
                            self.fileList.append(f)
                        if(splt[1] == "h"):
                            self.fileList.append(f)
                        if(splt[1] == "hpp"):
                            self.fileList.append(f)
            


    def run(self):     
            for filepath in self.fileList:
                fin = open(filepath, "r")
                fout = open(filepath + ".txt", "w")
                content = fin.readlines()
                
                for line in content:
                    outline = line
                    for sstr in self.sstr:
                        for post in self.postdelim:
                            for pre in self.predelim:
        
                                cmpstr = (pre + sstr.find + post)
                                replacestr = (pre + sstr.replacewith + post)
                                

                                outline = outline.replace(cmpstr, replacestr)

                            if(outline.find(sstr.find + post) == 0):
                                outline = re.sub(sstr.find + post, '', outline)
                                outline = sstr.replacewith + post + outline
                    fout.write(outline)
                fin.close()
                fout.close()
                
                         
                print("Finished file: " + filepath)
            
            print("Completed processing")

            
    def copy(self):
        for filepath in self.fileList:
            src = filepath + ".txt"
            shutil.copy2( src , filepath )
            sleep(0.1)
            print("copied file " + filepath)
        print("Completed copy")

    def remove_processed(self):
        for filepath in self.fileList:
            src = filepath + ".txt"
            os.remove(src)
            sleep(0.05)
            print("removed file " + src)
        print("Completed remove")



if __name__ == '__main__':

    from optparse import OptionParser
    parser = OptionParser("pypolar.py [options]")

    parser.add_option("--directory", dest="directories", action='append', help="search directory")

    parser.add_option("--recursive", dest="recursive", help="File search is recursive", action='store_true', default=False)

    parser.add_option("--test", dest="test", help="File search is recursive", action='store_true', default=False)

    (opts, args) = parser.parse_args()
    
    sstr = []
    pre = []
    post = []
    
    sstr.append(searchstr("long long", "int64_t"))
    sstr.append(searchstr("unsigned long", "uint32_t"))
    sstr.append(searchstr("long", "int32_t"))
    sstr.append(searchstr("unsigned int", "uint16_t"))
    sstr.append(searchstr("int", "int16_t"))
    sstr.append(searchstr("unsigned short", "uint16_t"))
    sstr.append(searchstr("short", "int16_t"))
    sstr.append(searchstr("unsigned char", "uint8_t"))
    
    pre.append(" ")
    pre.append("(")
    pre.append("\t")
    
    post.append(" ")
    post.append(")")
    post.append("\t")
    post.append("*")
    
    rootdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "..")

    rep = replacer(rootdir, opts, sstr, pre, post)
    rep.find_files()
    
    action = ""
    
    while(action != "X"):
        action = raw_input(" (P)rocess\n (C)opy processed files to \n (R)emove processed files\n e(X)it\n")
        action = action.upper()
        
        if(action == 'P'):
            rep.run()
        if(action == 'R'):
            rep.remove_processed()
        if(action == 'C'):
            rep.copy()
    