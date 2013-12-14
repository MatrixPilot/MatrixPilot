#!/usr/bin/python

import sys

def str_to_dec(str):
	d = 0
	for i in range(len(str)):
		if str[i] == '1' :
			d += 1<<(7-i)
		elif str[i] != '0' :
			return -1
	return d



outFile = open('font_data.h', 'w')

outFile.write("const unsigned char font_data[] = {\n\t")

with open('mp-osd-font.mcm', 'r') as inFile:
	c = 0
	for line in inFile:
		d = str_to_dec(line.strip())
		if d > -1:
			outFile.write(hex(d)+", ")
			c += 1
			if c == 16:
				outFile.write("\n\t")
				c = 0

outFile.write("0x00 } ;\n")
