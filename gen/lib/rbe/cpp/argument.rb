
require 'b/cpp/type'

module RBe
  module Cpp
    class Argument
      attr_accessor :type, :name, :value, :out

      def initialize(type, name, value)
        @type, @name, @value = type, name, value
      end

      def self.parse(str)
        type_and_name, value = str.split('=').map{|x| x.strip}
        /([a-zA-Z0-9_]+)$/ =~ type_and_name
        name = $1.strip
        type = Type.normalize $`
        value = nil if value.nil? or value.empty?
        self.new(type, name, value)
      end
    end
  end
end
