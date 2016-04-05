RBe: Ruby向けHaiku C++ API バインディング
=========================================

RBeについて
-----------

RBeは、Haiku C++ APIのRubyバインディングです。このソフトウェアの最初の
目標は、Application KitとInterface KitのAPIをRubyから利用できるようにす
ることです。

前提ソフトウェア
----------------

* Ruby 2.3.0 
* Haiku OS x86 gcc2 hybrid hrev50101

ビルド方法
----------

### Ruby 2.3.0のインストール

1. Ruby 2.3.0のソースコードをRubyのウェブサイトから入手し、適当なディレ
   クトリに展開してください。
2. gcc4モードへ移行してください。
3. Rubyをビルドしてインストールしてください。
   注: --prefixを指定しないと、"make install"で失敗するようです。
4. rubyにパスを通してください。

### RBeのビルド

    > git clone https://github.com/nmbits/rbe.git
    > cd rbe
    > rake
    > ruby extconf.rb
    > make

RBeを試す
---------

### irbを使う

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

### exampleディレクトリ

exampleに、簡単な例があるので試してみてください。
