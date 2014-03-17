#/bin/sh

make
chmod 777 libpoly.a
chmod 777 include/poly.h
sudo mv libpoly.a /usr/local/lib/
sudo cp include/poly.h /usr/local/include/
