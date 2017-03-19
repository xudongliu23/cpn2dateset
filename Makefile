all: main clean

main: main.o prefnet.o cpnet.o cpnet_instance.o
	g++ -w -std=c++11 -o main main.o prefnet.o cpnet.o cpnet_instance.o -lxerces-c

main.o: main.cxx cpnet.hxx cpnet_instance.hxx
	g++ -w -std=c++11 -I/home/xudong/Downloads/xsd-4.0.0-x86_64-linux-gnu/libxsd -c main.cxx

cpnet.o: cpnet.hxx cpnet_instance.hxx
	g++ -w -std=c++11 -I/home/xudong/Downloads/xsd-4.0.0-x86_64-linux-gnu/libxsd -c cpnet.cxx

prefnet.o: prefnet.hxx cpnet.hxx
	g++ -w -std=c++11 -I/home/xudong/Downloads/xsd-4.0.0-x86_64-linux-gnu/libxsd -c prefnet.cxx

cpnet_instance.o: cpnet_instance.hxx
	g++ -w -std=c++11 -I/home/xudong/Downloads/xsd-4.0.0-x86_64-linux-gnu/libxsd -c cpnet_instance.cxx

clean:
	rm *.o
