
require 'rbe/cpp/type'
require 'rbe/cpp/variable'

module RBe
  module Cpp
    class Interface
      attr_accessor :function, :default
      attr_reader :options, :ret, :args

      def initialize(args, ret, opts = {})
        @function = nil
        @ret = ret
        @args = args
        @options = opts
        @default = nil
      end

      def proto(with_function = true, with_class = false, for_impl = false)
        if with_function
          raise "This interface is not a member of any functions." unless @function
          function_name = @function.name
        end
        case with_class
        when true
          raise "This function is not a member of any classes." unless @function.clazz
          class_name = @function.clazz.name.sub(/^B/, '')
        when String
          class_name = with_class
        else
          class_name = nil
        end
        virtual = virtual? && !for_impl ? "virtual " : ""
        const = const? ? " const" : ""
        args_str = @args.each.map {|a| "#{a.type} #{a.name}" }.join ", "
        ret = @ret || 'void'
        if class_name
          "#{virtual}#{ret} #{class_name}::#{function_name}(#{args_str})#{const}"
        else
          "#{virtual}#{ret} #{function_name}(#{args_str})#{const}"
        end
      end

      def to_ruby
        args = @args.map{|a| "\"#{a.to_ruby}\"" }.join(", ")
        answer = "[#{args}]"
        if ret?
          answer += (@default ? ", \"#{@ret} = #{@default}\"" : ", \"#{@ret}\"")
        else
          answer += ", nil"
        end
        unless @options.empty?
          answer += ", " + @options.keys.map{|o| ":#{o}"}.join(", ")
        end
        answer
      end

      def set_option(name, value = true)
        @options[name] = value
      end

      def option(name)
        @options[name]
      end

      [:virtual, :const, :ignore].each do |sym|
        define_method(sym.to_s + "?") { option sym }
      end

      def types
        answer = @args.map {|a| a.type }
        answer << @ret if ret?
        answer
      end

      def arity
        @args.length
      end

      def arity_min
        args.count {|a| !a.optional? }
      end

      def method_arity
        args.count {|a| !a.out? && !a.pair? }
      end

      def method_arity_min
        args.count {|a| !a.out? && !a.pair? && !a.value }
      end

      def method_arity_optional
        method_arity - method_arity_min
      end
      
      def void?
        @ret.nil? || @ret.empty? || @ret.void?
      end

      def ret?
        ! void?
      end

      def each_method_arg_with_type
        first = nil
        i = 0
        @args.each do |a|
          if a.pair?
            first = a
            next
          end
          if first
            yield first, Type.new("std::pair<#{first.type}, #{a.type}>"), i, a
            first = nil
          else
            yield a, a.type, i
          end
          i += 1
        end
      end
    end
  end
end
