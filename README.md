RBe: A Haiku C++ API bindings for Ruby
======================================

About RBe
---------

RBe is a Haiku C++ API bindings for Ruby(MRI).
The goal of this project is to make
Application kit and Interface kit callable from your ruby scripts.

A part of classes are supported for now.

Prerequisite
------------

* Ruby 2.3.0 
* Haiku OS x86 gcc2 hybrid hrev50101

How to build
------------

### Install Ruby 2.3.0

1. Get Ruby 2.3.0 tar ball from the web site. Then, build and install it with x86 arch.
2. Add your ruby's bin directory to PATH environment variable.

### Build RBe

    > git clone https://github.com/nmbits/rbe.git
    > cd rbe
    > rake
    > ruby extconf.rb
    > make

'make install' is not recommended.
