SHELL := /bin/bash

ifeq ($(shell uname),Darwin) #if using a Mac
  XSD_INCLUDE_FLAG:=-I/usr/local/opt/xsd-4.0.0-i686-macosx/libxsd
  ifeq ($(shell hostname -s),Xudongs-MacBook-Pro) #if using my MBP laptop
  USER_DIR:=/Users/xudong
  else ifeq ($(shell hostname -s),CIS-3211-64950)  #if using my office desktop
  USER_DIR:=/Users/n01237497
  else
  USER_DIR:=
  endif
else #if using my Linux laptop
  XSD_INCLUDE_FLAG:=-I$(USER_DIR)/Downloads/xsd-4.0.0-x86_64-linux-gnu/libxsd
  USER_DIR:=/home/xudong
endif


all: main clean

main: main.o prefnet.o cpnet.o cpnet_instance.o
	g++ -w -std=c++11 -o main main.o prefnet.o cpnet.o cpnet_instance.o -lxerces-c

main.o: main.cxx cpnet.hxx cpnet_instance.hxx
	g++ -w -std=c++11 $(XSD_INCLUDE_FLAG) -c main.cxx

cpnet.o: cpnet.hxx cpnet_instance.hxx
	g++ -w -std=c++11 $(XSD_INCLUDE_FLAG) -c cpnet.cxx

prefnet.o: prefnet.hxx cpnet.hxx
	g++ -w -std=c++11 $(XSD_INCLUDE_FLAG) -c prefnet.cxx

cpnet_instance.o: cpnet_instance.hxx
	g++ -w -std=c++11 $(XSD_INCLUDE_FLAG) -c cpnet_instance.cxx

clean:
	rm *.o
