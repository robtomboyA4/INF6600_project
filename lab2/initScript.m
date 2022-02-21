clear all
close all
clc

run('../truetime-2.0/init_truetime.m')  %pour initialiser les variables truetime


mex -setup:'C:\Program Files\MATLAB\R2021a\bin\win64\mexopts\mingw64_g++.xml' C++

%make_truetime          %pour la compilation de la librairie truetime
ttmex ctrlSM.cpp     %pour la compilation du code du fichier c++

%truetime

run('init_values.m')    %pour initialiser les valeurs du système