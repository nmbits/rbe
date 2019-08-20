RBe: Ruby向けHaiku C++ API バインディング
=========================================

RBeについて
-----------

RBeは、Haiku C++ APIのRubyバインディングです。現在のところサポートするクラスは
少数ですが、ウィンドウの表示やメッセージのハンドリングを試すことができます。

前提ソフトウェア
----------------

* Haiku OS R1/Beta1 x86 64-bit (32-bitも多分大丈夫)
* Ruby 2.6.3 or higher

ビルド方法
----------
 1. rubyとruby_develパッケージをpkgmanを使ってインストールする
 2. このリポジトリをローカルにクローンし、そのディレクトリへ移動する
 3. Run 'rake'.
 4. Run 'ruby extconf.rb'.
 5. Run 'make'. 'rbe.so'ができる。

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

```
 > cd example
 > ruby test4.rb
```
