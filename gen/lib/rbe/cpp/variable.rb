
require 'rbe/cpp/type'

module RBe
  module Cpp
    class Variable
      attr_accessor :type, :name, :value, :out, :accept_nil

      def initialize(type, name, value = nil)
        @type, @name, @value = type, name, value
      end

      def has_value?
        (!@value.nil?)
      end

      def deref_type
        if type =~ /\*$/
          return $`.strip
        end
        raise "#{type} is not a pointer type"
      end

      # assume
      # <type> <name> [ = <value> ]

      def self.parse(str)
        str.gsub!(/;$/, "")
        type_and_name, avalue = str.split('=').map{|x| x.strip}
        no_const = type_and_name.gsub(/const/, "")
        if no_const.split(/\s/).size == 1
          name = nil
          type = Type.normalize type_and_name
        else
          /([a-zA-Z0-9_\[\]]+)$/ =~ type_and_name
          name = $1
          type = Type.normalize $`
        end
        value = (avalue.nil? || avalue.empty? ? nil : avalue.strip)
        self.new(type, name, value)
      end
    end
  end
end
