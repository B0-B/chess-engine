@echo off 
chcp 65001
g++ -c .\src\main.cpp -o .\bin\main.o
g++ .\bin\main.o -o .\bin\main
.\bin\main.exe              