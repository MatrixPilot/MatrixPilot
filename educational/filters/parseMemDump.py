#!/usr/bin/env python

import fileinput
import sys
import math
from optparse import OptionParser

class ParseMemDump():
    def __init__(self, filename):
        self.filename = filename
        
    def run(self):
        ofileName = "%s.csv" % (self.filename)
        fileOut = open(ofileName, 'w')
        
        with open(self.filename + '.txt', 'r') as fileIn:
            for line in fileIn:
                # write a single column of hex values
                words = line.split()
                for word in words:
                    x = int(word, 16)
                    out = "%i\n" % x
                    fileOut.write(out)
                
def main(args):
    usage = "%s -i input_file_name"
    optParser = OptionParser(usage=usage % sys.argv[0])
    optParser.allow_interspersed_args = False
    optParser.add_option('-i', '--infile', dest="infile",
                      help="Convert <infile> txt to csv", default=None)

    if not sys.argv[1:]:
        optParser.print_usage()
        sys.exit(2)

    options, args = optParser.parse_args()

    options.outfile = '%s.csv' % options.infile

    sys.argv[:] = args

    print "parseMemDump(%s)" % (options.infile)

    parser = ParseMemDump(options.infile)
    parser.run()
    
if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
