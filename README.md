aram
======

Audio Recorder And Music


======
Build instruction

$ autoreconf -i <br/>
$ ./configure <br/>
$ ./odb-compile.sh <br/>
$ make <br/>

= dependencies =

- JACK, the audio engine <br/>
  Eg. $ sudo apt-get install libjack-jackd2-dev

- sqlite3, required by libodb-sqlite-2.3.0 (see below) <br/>
  Eg. $ sudo apt-get install libsqlite3-dev

- codesynthesis odb (http://www.codesynthesis.com/products/odb/download.xhtml) <br/>
  odb compiler binary: <br/>
    odb-2.3.0-x86_64-linux-gnu (its bin directory must be in PATH) <br/>
  common odb runtime library source (./configure, make, make install): <br/>
    libodb-2.3.0 <br/>
  sqlite3 odb runtime library source (./configure, make, make install): <br/>
    libodb-sqlite-2.3.0 <br/>
  You may have to add /usr/local/lib to your ldconfig.
