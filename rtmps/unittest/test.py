#!/usr/bin/env python

import signal
import time
import os
import sys
import re
import fileinput
import getopt
import struct

def get_path_of(filename):
    path = os.path.realpath(filename)
    if not os.path.isdir(path):
        path = os.path.dirname(path)
    return path

def newest_file_in(path):
    mtime = lambda f: os.stat(os.path.join(path, f)).st_mtime
    ls = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
    newest_file_name = list(sorted(ls, key=mtime))[-1]
    return os.path.join(path, newest_file_name)

def exist(filename):
    return os.path.isfile(filename) or os.path.islink(filename)
    
def test(filename):
  print "filename>", filename
  index = 1

  current_file = filename + "."+ str(index)

  print "open>", current_file

  try:
    f = open(current_file, 'rb')

    while True:
      b = f.read(9)
      if b == '': break

      #print '%02X' % int(ord(b))
      print repr(b)
      #print struct.unpack('hhl', b)

      type= b[0:1]
      timestamp = b[1:5]
      length = b[5:9]
      
      print "type:" , repr(type)
      print "timestamp:" , repr(timestamp)
      print "length:" , repr(length)

    f.close()
    
  except IOError:
    print "open fail"
    return

  return


def sig_handler(signal, frame):
  sys.exit(0)
  return

def usage():
  print 'Usage: %s [option] FILE' % os.path.basename(sys.argv[0])
  print ' or :  %s [option] DIRECTORY' % os.path.basename(sys.argv[0])
  return

def main():
  signal.signal(signal.SIGINT, sig_handler)

  filename='.'

  if len(sys.argv) >= 2:
    try:
      options, opt_args = getopt.getopt(sys.argv[1:], "h", ["help"])
    except:
      usage()
      sys.exit(1)  

    for op, p in options:
      if op=="-h" or op=="--help":
        usage()
        sys.exit(1)
    if len(opt_args) > 0:
      filename=opt_args[0] 

  test(filename)
  return

if __name__ == '__main__':
  main()



