
require 'rbe/cpp/type'
require 'rbe/cpp/variable'

module RBe
  module Cpp
    class Function
      attr_reader :name, :args, :ret, :option
      attr_accessor :index, :clazz, :ret_default

      def initialize(name, args, ret, option)
        @clazz = nil
        @name = name
        @args = args
        @ret = ret
        @index = nil
        @option = option
      end

      alias_method :class_def, :clazz

      def proto
        unless @proto
          virtual = virtual? ? "virtual " : ""
          const = const? ? " const" : ""
          args_str = @args.each_with_index.map{|a, i| "#{a.type} #{a.name}" }.join ", "
          @proto = "#{virtual}#{@ret} #{@name}(#{args_str})#{const}"
        end
        @proto
      end

      def has_option?(opt)
        @option.include? opt
      end

      def set_option(opt, value)
        @option[opt] = value
      end

      def const?
        @option[:const]
      end

      def virtual?
        @option[:virtual]
      end

      def custom?
        @option[:custom]
      end

      def custom_hook?
        @option[:custom_hook]
      end

      def noimp?
        @option[:noimp]
      end

      def assert_locked?
        @option[:assert_locked]
      end

      def static?
        @option[:static]
      end

      def arity
        @args.length
      end

      def arity_min
        args.count{ |a| a.value.nil? }
      end

      def void?
        @ret == 'void'
      end

      def ret?
        @ret != 'void'
      end

      def self.parse(str)
        opt = {}
        if str.include? "operator"
          return :maybe_operator, nil
        end
        /\((.*)\)/ =~ str.strip
        ret_and_fname = $`.strip
        args = $1.strip
        const = $'.strip
        ret_and_fname.sub! /inline/, ""

        if const.include? "const"
          opt[:const] = true
        end

        /([a-zA-Z0-9_]+)$/ =~ ret_and_fname
        fname = $1
        ret = $`.strip
        if ret.include? "~"
          # destructor
          return :destructor, nil
        elsif ret.include? "virtual"
          opt[:virtual] = true
          ret.gsub!(/virtual/, "").strip!
        elsif ret.include? "static"
          opt[:static] = true
          ret.gsub!(/static/, "").strip!
        end

        ret = Type.normalize ret
        xargs = args.split(',').each_with_index.map do |a, i|
          v = Variable.parse a
        end
        function = self.new(fname, xargs, ret, opt)
        what = (ret.empty? ? :constructor : :function)
        return what, function
      end
    end
  end
end
