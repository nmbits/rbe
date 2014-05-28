
module RBe
  module Cpp
    class Type
      def self.normalize(type)
        x = type.dup
        x.gsub!(/\s+/, ' ')
        x.gsub!(/\*\s*/, '*')
        x.gsub!(/([a-zA-Z0-9_])\s*\*/, "\\1 *")
        x.strip!
        x
      end
    end
  end
end
