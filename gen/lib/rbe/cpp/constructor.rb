
require 'rbe/cpp/function'

module RBe
  module Cpp
    class Constructor < Function
      def initialize(args)
        @args = Function.parse_args(args)
        @option = {}
      end

      def ret?
        false
      end
    end
  end
end
