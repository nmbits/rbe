
RBeについて
===========

RBeは、Haiku C++ APIのRubyバインディングです。このソフトウェアの最初の
目標は、Application KitとInterface KitのAPIをRubyから利用できるようにす
ることです。

このバージョンは、Application Kitの一部のみをサポートしています。

前提ソフトウェア
================

 - Ruby 2.1.0 
 - Haiku OS x86 gcc2 hybrid hrev46924

 注: これらより新しいバージョンではテストしていません。


ビルド方法
==========

Ruby 2.1.0のインストール
------------------------

1. Ruby 2.1.0のソースコードをRubyのウェブサイトから入手し、適当なディレ
   クトリに展開してください。

2. ruby-2.1.0-haiku-thread.patchを、展開したディレクトリに適用してくだ
   さい。

   > patch < ruby-2.1.0-haiku-thread.patch

3. gcc4モードへ移行してください。

   > setarch x86

4. Rubyをビルドしてインストールしてください。

   > ./configure --prefix=/boot/home/ruby-2.1.0

   注: --prefixを指定しないと、"make install"で失敗するようです。

5. rubyにパスを通してください。

   > export PATH=$PATH:/boot/home/ruby-2.1.0/bin

RBeのビルド
-----------

1. RBeのソースを適当なディレクトリに展開し、そのディレクトリへ移動して
   ください。

2. genディレクトリへ移動し、rakeコマンドを実行してください。

  > cd gen
  > rake

3. RBeのソースのディレクトリへ戻り、extconf.rbを実行してください。

  > cd ..
  > ruby extconf.rb

4. makeを実行してください。rbe.soが作られます。

  > make

  注: "make install"するのはお勧めしません。


RBeの試し方
===========

irbを使う
---------

irbを使うことで、RBeを簡単に試せます。以下は、irbのセッションの例です。

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

exampleディレクトリ
-------------------

exampleに、簡単な例があるので試してみてください。


プラン
======

V0.1: 実現性調査 (このバージョン)
V0.4: Application Kitサポート
V0.6: Interface Kitの一部をサポート。Art of BeOS Programmingの例を実装。
V1.0: Interface Kitサポート
