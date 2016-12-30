all: libosmadr.so osmadr

libosmadr.so: libosmadr.o overpass.o
	g++ -shared -Wl,-soname,libosmadr.so.1 -o libosmadr.so  libosmadr.o overpass.o

libosmadr.o: libosmadr.cc
	g++ -Wall -c -fPIC libosmadr.cc -o libosmadr.o

overpass.o: overpass.cc
	g++ -Wall -c -fPIC overpass.cc -o overpass.o

clean:
	rm -rf *.o libosmadr.so osmadr

osmadr: osmadr.o libosmadr.so
	g++ -L. -losmadr -o osmadr osmadr.o

osmadr.o: osmadr.cc
	g++ -Wall -c -o osmadr.o osmadr.cc

lib_install: libosmadr.so
	cp ./libosmadr.so /usr/local/lib64/libosmadr.so.1.0
	ln -sf /usr/local/lib64/libosmadr.so.1.0 /usr/local/lib64/libosmadr.so
	ldconfig

lib: libosmadr.so

install: all
	make lib_install
	cp osmadr /usr/local/bin/osmadr
