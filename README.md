redchat
=======

Simple chat console application made as an assignment for the Computer
Networking class at University of São Paulo, São Carlos campus.

This project attempts to show socket programming in C with the
concepts of sockets and TCP learned, mixed with threads from the
Operating Systems class.

Dependencies
------------

GNU build essentials `gcc` and `make`.

Building
--------

Building is as simple as running make on this folder after cloning the
repo.

    make

Debug options with optional colorized output are available by editing the macro on `include/redchat.h`. A few other options can be changed there as well, such as number of contacts, stored messages and etc.
Error return codes are also located on `redchat.h`.

Running
-------

To run, after building, run the executable file located on `bin`.

    bin/redchat

Usage
-----

A menu is presented when running the application, just browse through
the options. It's pretty straight-forward.

To do
-----

This list. Also, a lot of improvements need to be made regarding
mutual exclusion.

License
-------
Code is licensed under the MIT license.
