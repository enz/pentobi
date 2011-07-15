#!/usr/bin/python
#
# Create a C++ include file that defines a char array constant with the content
# of a text file. This script is used to convert opening book SGF files into
# built-in books.
#
# Arguments: the variable name.
# The resulting file will have a line starting with
#   const unsigned char <variable_name>[] =
# followed by the contents of the file converted into a char array with an
# additional null char at the end

from sys import argv, stdin

print "// Created by tools/misc/text2include.py"
print "const char %s[] =" % argv[1]
print "{",
n = 0
while True:
    s = stdin.read(1)
    if (s == ""):
        break
    print "%s," % hex(ord(s[0])), 
    n = n + 1
    if n % 10 == 0:
        print
        print " ",
print "0x0 };"
