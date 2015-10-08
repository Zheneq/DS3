set step=1
set fps=25
IF "%4"=="-fast" set step=10
IF "%4"=="-fast" set fps=10

::mkdir %1\figures
::mkdir %1\figures-spec
::mkdir %1\%1-subimpulses
mkdir %1\%1-records

::gnuplot -e "begin='0'; end='%2'; step='%step%'; folder='%1'; leftrange='%5'; rightrange='%6'" animator.txt
::IF "%3"=="-spec" gnuplot -e "begin='0'; end='%2'; step='%step%'; folder='%1'" animator-spec.txt

::gnuplot -e "folder='%1'" a-finale.txt
gnuplot -e "begin='0'; end='%2'; step='%step%'; folder='%1'; leftrange='%5'; rightrange='%6'" a-finale.txt

for %%p in (%1\e*.txt) do del /s /q "%%p"
for %%p in (%1\h*.txt) do del /s /q "%%p"

::ffmpeg -r 25 -i "%1\figures\frame_%%06d.png" -vcodec mpeg4 -b:v 4000000 "%1\%1.avi"
::IF "%3"=="-spec" ffmpeg -r 25 -i "%1\figures-spec\frame_%%06d.png" -vcodec mpeg4 -b:v 4000000 "%1\%1-spec.avi"

for %%p in (%1\figures\*) do del /s /q "%%p"
for %%p in (%1\figures-spec\*) do del /s /q "%%p"
rmdir %1\figures
rmdir %1\figures-spec

gnuplot -e "folder='%1'; time='%7'" a-nrg.txt