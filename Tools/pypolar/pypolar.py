#!/usr/bin/env python

#import scipy
#import numpy

import re, os, sys

class polar_point(object):
    def __init__(self):
        self.alpha = 0
        self.Cl = 0
        self.Cd = 0
        self.Cm = 0

class polar(object):
    def __init__(self):
        self.Re     = 0
        self.flap   = 0
        self.points = []


class polar_parser(object):
    def __init__(self, directory, airfoil):
        self.directory = directory
        self.airfoil = airfoil
        self.polars = []
        
    def parse_xflr_all(self):
        self.polars = []
        for fname in os.listdir(self.directory):
            if fname.startswith(self.airfoil):
                filepath = os.path.join(self.directory, fname)
                self.parse_xflr_polar(filepath, fname)
        
    def parse_xflr_polar(self, filepath, filename):
        temppolar = polar()
        degpos = filename.find("deg")
        if(degpos == -1):
            temppolar.flap = 0
        else:
            splt = filename.split("deg")
            splt = splt[0].split(self.airfoil)
            temppolar.flap = float(splt[1])           
        f = open( filepath )
        lines = f.readlines()
        index = 0
        for line in lines:
            if (line.find("Mach =") != -1 ):
                splt = line.split("Re =")
                splt = splt[1].split("Ncrit")
                splt = splt[0]
                splt = splt.replace(" ", "")
                temppolar.Re = float(splt)
            if (line.find("-----") != -1 ):
                data_start = index
            index = index + 1
        f.close()
        self.polars.append(temppolar)

if __name__ == '__main__':

    from optparse import OptionParser
    parser = OptionParser("pypolar.py [options]")

    parser.add_option("--airfoil",dest="airfoil", help="airfoil name", default="foil")

    parser.add_option("--directory",dest="directory", help="directory path", default="")

    (opts, args) = parser.parse_args()
    
    if(opts.directory== ""):
        directory = os.path.dirname(os.path.realpath(__file__))
    else:
        directory = os.path.join(os.path.dirname(os.path.realpath(__file__)), opts.directory) 

    pparser = polar_parser(directory, opts.airfoil)
    pparser.parse_xflr_all()
    
    
            
