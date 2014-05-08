from numpy import std,mean,median
import json

TEST_DATA = 'map_point_apr_08'

x = open(TEST_DATA)
lines = []
for line in x :
    lines.append(eval(line))

x_values = {}
y_values = {}

for i in xrange(len(lines)):
    x_values[i] = ((lines[i]['x']))
    y_values[i] = ((lines[i]['y']))

temp = []
for j in range(5):
    temp.append(x_values[j])
    print temp[j]

print "mean %s"%mean(temp)
print "median %s"%median(temp)

#print y_values
'''
print median(y_values)
print median(x_values)
print mean(y_values)
print std(x_values)
'''
