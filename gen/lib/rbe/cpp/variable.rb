
require 'rbe/cpp/type'

module RBe
  module Cpp
    class Variable < Struct.new(:type, :name, :value, :options)
      OPTIONS = ["out", "release", "own", "nillable", "pair"]

      def has_value?
        ! value.nil?
      end

      def option(sym)
        options[sym]
      end
      
      OPTIONS.each do |opt|
        define_method(opt + "?") { option opt.to_sym }
      end

      def optional?
        value ? true : false
      end
      
      def to_ruby
        if options && !options.empty?
          answer = options.keys.map{|o| '%' + o.to_s}.join(" ") + " "
        else
          answer = ""
        end
        answer += type
        answer += " " + name if name
        answer.gsub!(/\* /, "*")
        if value
          answer += " = " + value
        end
        answer
      end

      alias_method :to_s, :to_ruby
      
      def self.parse(str)
        options = {}
        str.gsub!(/;$/, "")
        type_and_name, avalue = str.split('=').map {|x| x.strip }
        # pure = type_and_name.gsub(/const/, "")
        pure = type_and_name.dup
        len = pure.length
        OPTIONS.each do |opt|
          pure.gsub!(/%#{opt}/, "")
          if pure.length < len
            options[opt.to_sym] = true
            len = pure.length
          end
        end
        if pure.split(/\W/).size == 1
          name = nil
          type = Type.new pure
        else
          /([a-zA-Z0-9_\[\]]+)$/ =~ pure
          name = $1
          type = Type.new $`
        end
        value = (avalue.nil? || avalue.empty? ? nil : avalue.strip)
        self.new(type, name, value, options)
      end
    end
  end
end
