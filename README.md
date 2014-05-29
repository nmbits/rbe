
About RBe
=========

RBe is a Ruby (MRI) binding for Haiku C++ API. The first goal is to
provide Application kit and Interface kit APIs to Ruby scripts.

This version supports a small part of Application kit.

Prerequisite
============

 - Ruby 2.1.0 
 - Haiku OS x86 gcc2 hybrid hrev46924

 note: RBe is not tested for later versions.

How to build
=============

Install Ruby 2.1.0
------------------

1. Get Ruby 2.1.0 tar ball from the Ruby web site. Expand it.

2. Apply ruby-2.1.0-haiku-thread.patch to the source tree.

   > patch < ruby-2.1.0-haiku-thread.patch

3. Switch to gcc4 mode.

   > setarch x86

4. Build and install Ruby.

   > ./configure --prefix=/boot/home/opt/ruby-2.1.0

   note: without --prefix option, "make install" may not success.

5. Add your ruby-2.1.0/bin directory to PATH environment variable.

   > export PATH=$PATH:/boot/home/opt/ruby-2.1.0/bin

Build RBe
---------

1. Put RBe source files into a direcotry, then cd there.

2. Move to gen directory, then execute rake.

  > cd gen
  > rake

3. Move back to RBe source direcotry, then execute extconf.rb.

  > cd ..
  > ruby extconf.rb

4. Run make. rbe.so will be made.

  > make

  note: It's not recommended to do "make install."


Testing RBe
===========

With irb
--------

You can test RBe by using irb. The following is an example of an irb
session.

  > irb -r ./rbe.so
  irb(main):001:0> l = B::Looper.new
  => #<B::Looper:0x11110000>
  irb(main):002:0> l.run
  => #<Thread:0x11112222 sleep>
  irb(main):003:0> def l.message_received(message)
  irb(main):004:1>   message.print_to_stream
  irb(main):005:1> end
  => :message_received
  irb(main):006:0> l.post_message 1
  BMessage(0x01)
  => nil
  irb(main):007:0> l.lock
  => true
  irb(main):008:0> l.quit
  => nil
  irb(main):009:0> quit
  >  


"example" directory
-----------------

Please try them.


Plan
====

V0.1: Feasibility study (this version)
V0.4: Application Kit support
V0.6: Partial Interface Kit support. Implementing examples from 'Art
      of BeOS Programming'
V1.0: Interface Kit support
