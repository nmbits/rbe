
module RBe
  module Cpp
    class Type < String
      def initialize(string)
        self.replace normalize_common(string)
      end

      def void?
        self == "void"
      end
      
      def normalize
        normalize_common self.dup
      end

      def normalize!
        normalize_common self
      end

      def dereferenced
        if /\*$/ =~ self
          return Type.new($`.strip)
        end
        raise "#{type} is not a pointer"
      end
      
      private

      def normalize_common(target)
        target.gsub!(/\s+/, ' ')
        target.gsub!(/\*\s*/, '*')
        target.gsub!(/([a-zA-Z0-9_])\s*\*/, "\\1 *")
        target.strip!
        target
      end
    end
  end
end
