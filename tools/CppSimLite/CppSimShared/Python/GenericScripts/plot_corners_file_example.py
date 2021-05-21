from __future__ import division
import numpy as np
from pylab import *
import csv
from collections import defaultdict
from scipy.signal import lfilter,welch,hann
from scipy import fft
import sys

# create convenience function for finding indexes in list items that match val
def find_ind(lst,val):
   result = []
   for i, x in enumerate(lst):
      if x == val:
          result.append(i)
   return result
# create convenience function for finding pairwise indexes in list items that match val
def find_ind_two(lst1,val1,lst2,val2):
   result = []
   for i, x in enumerate(lst1):
      if i < len(lst2):
         y = lst2[i]
      else:
         y = 'null'
      if x == val1 and y == val2:
          result.append(i)
   return result
# create convenience function for creating new list corresponding to input index list
def choose_ind(lst,ind_lst):
   result = []
   for i in ind_lst:
      result.append(lst[i])
   return result
    

file_name = 'corners.txt'
columns = defaultdict(list) # values for each column are appended to a list
signames = defaultdict(list) # signal names for each column are stored in a list
with open(file_name) as f:
   reader = csv.reader(f,delimiter='\t')
   # obtain title from first row
   title_txt = next(reader)
   # obtain column names from second row
   fields = next(reader)
   for (i,v) in enumerate(fields):
      signames[i] = v
   # obtain data for each column
   for row in reader:
     for (i,v) in enumerate(row):
        columns[i].append(v)
   f.close()

## print examples
# print title
print('Title: %s' % title_txt[0])
# print 6th and 7th column names as an example
print('col names:  5: %s  6: %s' % (signames[5], signames[6]))

## assign string column data
fab = columns[0]
mos = columns[1]
res = columns[2]
bjt = columns[3]
mim = columns[4]

# assign numerical column data to Python numpy arrays
vddscale = np.array(list(map(float,columns[5])))
ibias = np.array(list(map(float,columns[6])))
temp = np.array(list(map(float,columns[7])))
pwrmode = np.array(list(map(float,columns[8])))
rl = np.array(list(map(float,columns[9])))
op = np.array(list(map(float,columns[10])))
on = np.array(list(map(float,columns[11])))
vbulk = np.array(list(map(float,columns[12])))
idd = np.array(list(map(float,columns[13])))
iss = np.array(list(map(float,columns[14])))

## Anaylze data and print results

meas_var = op
meas_var_name = 'OP'

print('\n------------- %s -------------' % meas_var_name)
ind = meas_var.argmin()
print('Min value = %7.4e' % (meas_var[ind]))
# below use " instead of ' since I use ' within print string
print("--> occurs at mos '%s', res '%s', bjt '%s', mim '%s', temp '%3.0f'\n" % (mos[ind],res[ind],bjt[ind],mim[ind],temp[ind]))

ind = meas_var.argmax()
print('Max value = %7.4e' % (meas_var[ind]))
# below use " instead of ' since I use ' within print string
print("--> occurs at mos '%s', res '%s', bjt '%s', mim '%s', temp '%3.0f'\n" % (mos[ind],res[ind],bjt[ind],mim[ind],temp[ind]))


meas_var = on
meas_var_name = 'ON'

print('\n------------- %s -------------' % meas_var_name)
ind = meas_var.argmin()
print('Min value = %7.4e' % (meas_var[ind]))
# below use " instead of ' since I use ' within print string
print("--> occurs at mos '%s', res '%s', bjt '%s', mim '%s', temp '%3.0f'\n" % (mos[ind],res[ind],bjt[ind],mim[ind],temp[ind]))

ind = meas_var.argmax()
print('Max value = %7.4e' % (meas_var[ind]))
# below use " instead of ' since I use ' within print string
print("--> occurs at mos '%s', res '%s', bjt '%s', mim '%s', temp '%3.0f'\n" % (mos[ind],res[ind],bjt[ind],mim[ind],temp[ind]))


## Create plots

fig = figure(1)
fig.clf()
# increase thickness of plot lines to 2
rcParams['lines.linewidth'] = 2


## plot OP versus MOS ff,ss,fs,sf
subplot(211)

plot_var = op
plot_var_name = 'OP'
corner_list = ['ss','ff','fs','sf']
plot_symbol = ['r*','bx','go','ms']
i = 0
for corner_val in corner_list:
  # example here uses two lines to subselect data - example below shows this operation in one line
  ind = find_ind(mos,corner_val)
  plot_var_sel = choose_ind(plot_var,ind)
  plot(plot_var_sel,plot_symbol[i],label='%s (%s)' % (plot_var_name, corner_val))
  i += 1

ylabel('V')
#xlabel('Sample')
grid(True)
title('%s versus corner' % (plot_var_name))
legend(loc='upper right',prop={'size':10})


## plot ON versus MOS ff,ss and temp -40,85
subplot(212)

plot_var = on
plot_var_name = 'ON'
corner_list = ['ss','ff']
temp_list = [-40, 85]
plot_symbol = ['r*','bx','go','ms']
i = 0
for corner_val in corner_list:
  for temp_val in temp_list:
     # use one line to subselect data instead of the example above which used two lines
     plot_var_sel = choose_ind(plot_var,find_ind_two(mos,corner_val,temp,temp_val))
     plot(plot_var_sel,plot_symbol[i],label='%s (%s %s)' % (plot_var_name, corner_val, temp_val))
     i += 1

ylabel('V')
xlabel('Sample')
grid(True)
# example of a different way of setting title string
title_str = '%s versus corner' % (plot_var_name)
title(title_str)
legend(loc='lower left',prop={'size':10})

fig.show()

