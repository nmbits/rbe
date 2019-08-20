RBe: A Haiku C++ API bindings for Ruby
======================================

About
-----

RBe is a Haiku C++ API bindings for Ruby(MRI).

Few classes are supported for now. However, you can display your window written in Ruby, having menu and text view, and handle mouse and keyboard messages, by using this library.

Prerequisite
------------

* Haiku OS R1/Beta1 x86 64-bit (32-bit might OK)
* Ruby 2.6.3 or higher

How to build
------------

 1. Install ruby and ruby_devel packages via pkgman.
 2. Clone this repositoy into your working directory and cd to there.
 3. Run 'rake'.
 4. Run 'ruby extconf.rb'.
 5. Run 'make'. 'rbe.so' is created.

```
 > pkgman install ruby ruby_devel
 > git clone https://github.com/nmbits/rbe.git
 > cd rbe
 > rake
 > ruby extconf.rb
 > make
```

Running example
---------------
 > cd example
 > ruby test4.rb
