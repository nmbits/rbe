
module RBe
  module Cpp
    class ClassList
      def initialize
        @classes = {}
      end

      def add(clazz)
        @classes[clazz.name] = clazz
        clazz.class_list = self
      end

      def [](name)
        @classes[name]
      end

      def each(&block)
        @classes.values.each(&block)
      end

      def list
        @classes.values
      end
    end
  end
end
