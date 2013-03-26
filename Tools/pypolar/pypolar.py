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
        self.Cdtot = 0
    
    def __str__(self):
        text = ("alpha={0} Cl={1} Cd={2} Cm={3}").format(self.alpha, self.Cl, self.Cd, self.Cm)
        return text
        
class polar(object):
    def __init__(self):
        self.Re     = 0
        self.flap   = 0
        self.points = []
        self.alpha_clmax    = 0;
        self.alpha_step     = 1;
        self.clmax_index    = 0;
        
    def debug_str(self):
        text = ("Re={0} flap={1}").format(self.Re, self.flap)
        print(text)
        if(len(self.points) > 0):
            text = ("Clmax={0} Clmaxalpha={1}").format(self.points[self.clmax_index].Cl, self.alpha_clmax)
            print(text)
        for pnt in self.points:
            print(pnt)

    def min_alpha(self):
        if(self.points[0]):
            return self.points[0].alpha
        else:
            return 0

    def find_alpha_clmax(self):
        Cl = self.points[0].Cl
        index = 0
        maxcl_index = 0;
        for pnt in self.points:
            if(pnt.Cl > Cl):
                Cl = pnt.Cl
                maxcl_index = index
            index = index + 1
        self.alpha_clmax = self.points[maxcl_index].alpha
        self.clmax_index = maxcl_index

class polar_parser(object):
    def __init__(self, directory, airfoil):
        self.directory = directory
        self.airfoil = airfoil
        self.polars = []

        self.Re_list = []
        self.flap_list = []

        self.bodyCd = 0

    def debug_str(self):
        print("airfoil= " + self.airfoil)
        print("directory= " + self.directory)
        for polar in self.polars:
            polar.debug_str()
        print(self.Re_list)
        print(self.flap_list)
            
    def process(self, bodycd):
        self.Re_list = []
        self.flap_list = []
        self.bodyCd = bodycd

        for plr in self.polars:
            if (plr.Re not in self.Re_list):
                self.Re_list.append(plr.Re)
            if (plr.flap not in self.flap_list):
                self.flap_list.append(plr.flap)
            for pnt in plr.points:
                pnt.Cdtot = pnt.Cd + self.bodyCd
            plr.find_alpha_clmax()
        
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
        f.close()
        index = 0
        for line in lines:
            if (line.find("Mach =") != -1 ):
                splt = line.split("Re =")
                splt = splt[1].split("Ncrit")
                splt = splt[0]
                splt = splt.replace(" ", "")
                temppolar.Re = float(splt)
            if (line.find("-----") != -1 ):
                data_start = index + 1
            index = index + 1
        for line in lines[data_start:len(lines)]:
                splt = re.sub(' +',' ',line)
                splt = splt.split(" ")
                if(len(splt) >= 5):
                    temppoint = polar_point() 
                    temppoint.alpha = float(splt[1])
                    temppoint.Cl = float(splt[2])
                    temppoint.Cd = float(splt[3])
                    temppoint.Cm = float(splt[5])
                    temppolar.points.append(temppoint)
        self.polars.append(temppolar)




if __name__ == '__main__':

    from optparse import OptionParser
    parser = OptionParser("pypolar.py [options]")

    parser.add_option("--airfoil",dest="airfoil", help="airfoil name", default="foil")

    parser.add_option("--directory",dest="directory", help="directory path", default="")

    parser.add_option("--bodyCd",dest="bodycd", help="Drag coefficient of body", type='float', default=0)

    (opts, args) = parser.parse_args()
    
    if(opts.directory== ""):
        directory = os.path.dirname(os.path.realpath(__file__))
    else:
        directory = os.path.join(os.path.dirname(os.path.realpath(__file__)), opts.directory) 

    pparser = polar_parser(directory, opts.airfoil)
    pparser.parse_xflr_all()
    pparser.process(opts.bodycd)
    
    pparser.debug_str()
            
