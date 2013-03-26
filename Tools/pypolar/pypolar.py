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
    
    def __str__(self):
        text = ("alpha={0} Cl={1} Cd={2} Cm={3}").format(self.alpha, self.Cl, self.Cd, self.Cm)
        return text
    
    ''' Interpolate between self and point b along alpha to point alpha'''
    def point_interpolate(self, alpha, pointb):
        pnt = polar_point()
        ratio = alpha - self.alpha
        ratio = ratio / (pointb.alpha - self.alpha)
        pnt.alpha = alpha
        pnt.Cl = (ratio * (pointb.Cl - self.Cl)) + self.Cl
        pnt.Cd = (ratio * (pointb.Cd - self.Cd)) + self.Cd
        pnt.Cm = (ratio * (pointb.Cm - self.Cm)) + self.Cm
        return pnt
        
class polar(object):
    def __init__(self):
        self.Re     = 0
        self.flap   = 0
        self.aspd   = 0
        self.points = []
        self.alpha_clmax    = 0
        self.alpha_step     = 1
        self.clmax_index    = 0
        self.name     = ""
        
    def calc_aspd_from_Re(self, MAC):
        self.aspd = self.Re / (70000 * MAC)
        return self.aspd
    
    # Make a nice human readable reference name for structures
    def gen_polar_name(self):
        if(self.flap == 0):
            flapdir = ""
            flap = 0
        elif(self.flap > 0):
            flapdir = "down"
            flap = self.flap
        else:
            flapdir = "up"
            flap = -self.flap
            
        self.name = ("aspd{0}_flap{1}{2}").format(int(self.aspd), flapdir, int(flap))
        
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
        
    def generate_missing_points(self, alpha_step):
        self.alpha_step = alpha_step
        index = 0
        for index in range(0, len(self.points)-1):
            try:
                lowpoint = self.points[index]
                highpoint = self.points[index + 1]
                lowalpha = lowpoint.alpha
                highalpha = highpoint.alpha
                if(highalpha > lowalpha + alpha_step):
                    pnt = lowpoint.point_interpolate(lowalpha + alpha_step, highpoint)
                    self.points.insert(index+1, pnt)
                    text = ("interpolated missing data for Re={0} flap={1} at alpha={2}").format(self.Re, self.flap , lowalpha + alpha_step )
                    print(text)
            except:
                print("interpolate fail")

class polar_parser(object):
    def __init__(self, directory, airfoil):
        self.directory = directory
        self.airfoil = airfoil
        self.polars = []

        self.Re_list = []
        self.flap_list = []
        self.aspd_list = []     # airspeeds corresponding to reynolds numbers from MAC

        self.bodyCd = 0

    def debug_str(self):
        print("airfoil= " + self.airfoil)
        print("directory= " + self.directory)
        for polar in self.polars:
            polar.debug_str()
        print(self.Re_list)
        print(self.flap_list)
        print(self.aspd_list)
            
    def process(self, bodycd, alpha_step, MAC ):
        self.Re_list = []
        self.flap_list = []
        self.aspd_list = []     # airspeeds corresponding to reynolds numbers from MAC
        self.bodyCd = bodycd

        for plr in self.polars:
            plr.generate_missing_points( alpha_step )
            aspd = plr.calc_aspd_from_Re(MAC)
            if (plr.Re not in self.Re_list):
                self.Re_list.append(plr.Re)
                self.aspd_list.append(aspd)
            if (plr.flap not in self.flap_list):
                self.flap_list.append(plr.flap)
            for pnt in plr.points:
                pnt.Cdtot = pnt.Cd + self.bodyCd
            plr.find_alpha_clmax()
            plr.gen_polar_name()
            
    def ftoQ16(self, num):
        return int((num * 65536) + 0.5)
    
    def aspd_to_cm(self, aspd):
        return int( (aspd * 100) + 0.5 )
    
    def find_polar(self, aspd, flap):
        if(aspd not in self.aspd_list):
            return None
        if(flap not in self.flap_list):
            return None
        for plr in self.polars:
            if( (plr.aspd == aspd) and (plr.flap == flap) ):
                return plr
        return None
            
    def export_to_c(self):
        filepath = os.path.join(self.directory, "polar_data.c")
        f = open(filepath, "w")
        f.write("// File automatically generated by pypolar - DO NOT EDIT\n")
        f.write("//\n\n")
        f.write('#include "../MatrixPilot/defines.h"\n')
        f.write('#include "airframe_options.h"\n')
        f.write('#include "airframe.h"\n')
                
        f.write("\n\n")
        for plr in self.polars:
            text = ("const polar_point afrm_polar_points_{0}").format(plr.name) + "[] = {\n"
            f.write(text)
            for pnt in plr.points:
                text = ("{:d} , {:d} , {:d} , {:d}").format(self.ftoQ16(pnt.alpha), self.ftoQ16(pnt.Cl), self.ftoQ16(pnt.Cd), self.ftoQ16(pnt.Cm) )
                f.write("{ " + text + " },\n")
            f.write("};\n")
            f.write("\n")
            f.write("\n")

        f.write("const polar2 afrm_ppolars[] = {\n")
        for aspd in self.aspd_list:
            for flap in self.flap_list:
                plr = self.find_polar(aspd, flap)
                if(plr == None):
                    raise Exception("Polar not found")
                f.write("{ " +  str(self.aspd_to_cm(plr.aspd)) )
                f.write(" , " + str(self.ftoQ16(plr.flap)) )
                f.write(" , " + str(len(plr.points)) )
                f.write(" , &afrm_polar_points_" + plr.name )
                f.write(" , " + str(plr.clmax_index) + " },\n" )
        f.write("};\n")
        
        f.close()
        
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

    parser.add_option("--alphaStep",dest="alphastep", help="Grid step size of alpha", type='float', default=1.0)

    parser.add_option("--MAC",dest="MAC", help="Mean aerodynamic chord", type='float')

    (opts, args) = parser.parse_args()
    
    if(opts.directory== ""):
        directory = os.path.dirname(os.path.realpath(__file__))
    else:
        directory = os.path.join(os.path.dirname(os.path.realpath(__file__)), opts.directory) 

    print("airfoil= " + opts.airfoil)
    print("sub directory= " + opts.directory)

    pparser = polar_parser(directory, opts.airfoil)
    pparser.parse_xflr_all()
    pparser.process(opts.bodycd, opts.alphastep, opts.MAC )
    pparser.export_to_c()
    
    pparser.debug_str()
            
