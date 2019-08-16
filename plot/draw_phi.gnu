#! /usr/bin/gnuplot
set terminal png
set output 'plot/images/phi.png'
set datafile separator "\t"

unset key
unset border
unset tics

# set border 4095
set xrange [0:5]
set yrange [0:5]
set zrange [0:1]
set samples 100
set isosamples 100
set ticslevel 0
set pm3d at b
# set hidden3d
# set dgrid3d 50,50,1
# unset surface

set pm3d flush begin
set palette
unset title
# splot 'triangle.dat'
# #show pm3d
# pause -1 "Hit return to continue"
plot for [i=0:2] "plot/data/phi.txt" using 1:($3==i?$2:1/0):4 with points ls i+4 lc palette
# splot 'plot/data/phi.txt' using 1:2:($3/10):4 palette
# splot sin(sqrt(x**2+y**2))/sqrt(x**2+y**2)


# ! display plot/images/phi.png

