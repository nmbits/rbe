
require 'rbe/cpp/class'
require 'rbe/cpp/function'
require 'rbe/cpp/interface'
require 'rbe/cpp/variable'

module RBe
  module Loader
    class FunctionLoader
      def initialize(function, &block)
        @function = function
        instance_eval &block
      end

      def interface(args, ret, *opts)
        hash = {}
        opts.each{|o| hash[o] = true }
        xargs = args.map {|a| Cpp::Variable.parse a }
        value = nil
        if ret
          type, value = ret.split(/=/)
          xret = Cpp::Type.new(type)
        else
          xret = nil
        end
        interface = Cpp::Interface.new xargs, xret, hash
        if value
          interface.default = value.strip
        end
        @function.add_interface interface
      end
      
      def options(*opts)
        opts.each {|o| @function.set_option(o) }
      end
    end

    class ClassLoader
      def initialize(clazz, &block)
        @clazz = clazz
        instance_eval &block
      end

      def function(name, &block)
        function = Cpp::Function.new name
        f = FunctionLoader.new function, &block
        @clazz.add_function function
      end

      def variable(str)
        variable = Cpp::Variable.parse(str)
        @clazz.add_variable variable
      end

      def kit(str)
        @clazz.kit = str
      end

      def header_file(str)
        @clazz.header_file = str
      end
      
      def options(*opts)
        opts.each {|sym| @clazz.set_option sym }
      end

      def super_class_name(name)
        @clazz.super_class_name = name
      end

      def mixin_name(name)
        @clazz.mixin_name = name
      end

      def self.define_class(name, &block)
        clazz = Cpp::Class.new name
        ClassLoader.new(clazz, &block)
        clazz
      end
    end

    def self.load_file(filename)
      code = File.read filename
      ClassLoader.instance_eval { binding.eval code, filename }
    end
  end
end

if __FILE__ == $0
  definition = ARGV[0]
  file = File.expand_path("../../defs/#{definition}", File.dirname(__FILE__))
  c = RBe::Loader.load_file(file)
end
