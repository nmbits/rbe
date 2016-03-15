
require 'rbe/cpp/interface'
require 'rbe/cpp/type'
require 'rbe/cpp/variable'

module RBe
  module Cpp
    class Function
      attr_reader :name, :interfaces, :options
      attr_accessor :index, :clazz, :ret_default

      def initialize(name)
        @clazz = nil
        @name = name
        @interfaces = []
        @options = {}
      end

      alias_method :option, :options

      def set_option(name, value = true)
        @options[name] = value
      end

      [:hook, :custom, :custom_hook, :ignore,
       :assert_locked, :check_locked, :check_owner_locked, :prelock,
       :ctor, :const, :virtual, :static, :public].each do |sym|
        define_method(sym.to_s + "?") do
          @options[sym]
        end
      end

      def match(condition = {})
        answer = true
        condition.each do |name, value|
          opt = @options[name] ? true : false
          if (opt && !value) || (!opt && value)
            answer = false
            break
          end
        end
        answer
      end

      def add_interface(interface)
        if interface.function
          raise "The interface is already a member of function: #{interface.proto}"
        end
        interface.function = self
        @interfaces << interface
      end

      def dependencies
        return [] if ignore?
        names = {}
        @interfaces.each do |i|
          next if i.ignore?
          i.types.each do |t|
            names[$1] = true if /(B[A-Z][a-zA-Z0-9_]*)/ =~ t
          end
        end
        names.keys
      end

      def arity_max
        @arity_max ||= @interfaces.map{|i| i.arity }.max
      end

      def arity_min
        @arity_min ||= @interfaces.map{|i| i.arity_min }.min
      end
      
      def num_optional_args
        arity_max - arity_min
      end

      def method_arity_max
        @method_arity_max ||= @interfaces.map{|i| i.method_arity }.max
      end

      def method_arity_min
        @method_arity_min ||= @interfaces.map{|i| i.method_arity_min }.min
      end

      def method_arity_optional
        method_arity_max - method_arity_min
      end

      def self.parse(str)
        opt = {}
        return :operator if str.include? "operator"
        /\((.*)\)/ =~ str.strip
        ret_and_fname = $`.strip
        args = $1.strip
        const = $'.strip
        ret_and_fname.sub! /inline/, ""

        opt[:const] = true if const.include? "const"

        /([a-zA-Z0-9_]+)$/ =~ ret_and_fname
        fname = $1
        ret = $`.strip

        return :dtor if ret.include? "~"

        static = false
        if ret.include? "virtual"
          opt[:virtual] = true
          ret.gsub!(/virtual/, "").strip!
        elsif ret.include? "static"
          static = true
          ret.gsub!(/static/, "").strip!
        end

        ret_type = Type.new ret
        xargs = args.split(',').each.map {|a| Variable.parse a }
        interface = Interface.new xargs, ret_type, opt

        if ret.empty?
          what = :ctor
        elsif static
          what = :static
        else
          what = :function
        end
        return what, fname, interface
      end
    end
  end
end

if __FILE__ == $0
  require 'pp'
  require 'rbe/cpp/class'
  require 'erb'

  # pp RBe::Cpp::Function.parse 'void a(int b = 29)'
  # pp RBe::Cpp::Function.parse 'void a(int out b = 29)'
  # pp RBe::Cpp::Function.parse 'static int *a(int out b = 29) const'
  # pp RBe::Cpp::Function.parse 'static int **a(int out b = 29) const'
  # pp RBe::Cpp::Function.parse 'a(int out b)'
  # pp RBe::Cpp::Function.parse 'operator=(int b)'

  def t0
    c = RBe::Cpp::Class.new("BHandler", "BArchivable")
    what, name, interface = RBe::Cpp::Function.parse 'virtual void MessageReceived(int a0, %out %forget %memorize BMessage * name = NULL) const'
    f = RBe::Cpp::Function.new name
    f.add_interface interface
    f.set_option :hook
    f.set_option :ignore
    c.add_function f
    puts "arity_min: #{f.arity_min}"
    what, name, interface = RBe::Cpp::Function.parse 'BHandler()'
    f = RBe::Cpp::Function.new name
    f.add_interface interface
    f.set_option :ctor
    c.add_function f
    # pp c
    # pp c.hooks
    # pp c.ctors
    # pp f.dependencies
    # pp c.dependencies
    pp c.find_functions(ignore: false).map{|f| f.name }
  end

  def t1
    path = File.expand_path("../../../class_def.erb", File.dirname(__FILE__))
    erb = ERB.new File.read(path), nil, '%'

    c = RBe::Cpp::Class.new("BHandler", "BArchivable")
    what, name, interface = RBe::Cpp::Function.parse 'virtual void MessageReceived(%out %forget %memorize BMessage * name = NULL) const'
    f = RBe::Cpp::Function.new name
    f.add_interface interface
    f.set_option :hook
    c.add_function f
    what, name, interface = RBe::Cpp::Function.parse 'BHandler()'
    f = RBe::Cpp::Function.new name
    f.add_interface interface
    f.set_option :ctor
    c.add_function f
    c.set_option :wrapper

    target = c
    print erb.result(binding)
  end

  t0
  t1
  
end
