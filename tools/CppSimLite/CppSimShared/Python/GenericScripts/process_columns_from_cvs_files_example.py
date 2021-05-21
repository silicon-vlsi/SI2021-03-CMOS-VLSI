from __future__ import division
import numpy as np
from pylab import *
import csv
from collections import defaultdict
from scipy.signal import lfilter,welch,hann
from scipy import fft
import sys


## some useful helper functions:
# finding indices in list that match val
def find_ind(in_list,val):
    result = []
    for i,x in enumerate(in_list):
       if x == val:
          result.append(i)
    return result
# find indices that match conditions for two lists
def find_ind2(in_list1, val1, in_list2, val2):
    result = []
    for i,x in enumerate(in_list1):
        if i < len(in_list2):
           y = in_list2[i]
        else:
           y = 'Null'
        if x == val1 and y == val2:
           result.append(i)
    return result
# return list corresponding to specified indices of input list
def choose_ind(in_list,ind):
    result = []
    for i in ind:
       result.append(in_list[i])
    return result


rms_list = []
gain_val = arange(12)
for cur_gain in gain_val:
   file_name = 'gain%d.csv' % cur_gain
   col_names = defaultdict(list) # the name of each column is appended to a list
   columns = defaultdict(list) # the data in each column is appended to a list
   with open(file_name) as f:
       # note that default delimiter between entries is comma, but an example of having tabs would be
       # reader = csv.reader(f,delimiter='\t')
       reader = csv.reader(f)
       # obtain signal names from first row corresponding to each column
       first_row = reader.next()
       # note that if first_row was one entry, you could do:  print("first row = '%s'" % first_row[0])
       for (i,v) in enumerate(first_row):
          col_names[i] = v
       # obtain data for each column
       for row in reader:
          for (i,v) in enumerate(row):
             columns[i].append(v)
   f.close()

   # print first two column names as an example
   print("col names -> 0: '%s', 1: '%s'" % (col_names[0], col_names[1]))

   # note that if column data is numeric, convert to Numpy array as shown below
   # -> if column data consist of strings, you can assign simply as:  var = columns[0]
   t = np.asarray(columns[1],dtype=np.float32)
   t_delta = lfilter([1,-1],[1],t)
   sig = np.asarray(columns[2],dtype=np.float32)
   rms_list.append(std(sig))

rms_vals = np.array(rms_list)

fig = figure(1)
fig.clf()

plot(gain_val,rms_vals,'bo');
ylabel('RMS Values')
grid(True)
title('Measured RMS Values vs Gain Setting');
