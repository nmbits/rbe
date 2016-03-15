
require 'rbe/gen/util'
require 'erb'

module RBe
  module Gen
    class Generator
      include Util
      ERBS = {}

      def self.generate(sym, *args)
        erb = ERBS[sym]
        unless erb
          filename = sym.to_s + ".erb"
          path = File.expand_path filename, File.dirname(__FILE__)
          erb = ERB.new File.read(path), nil, '%'
          erb.filename = path
          ERBS[sym] = erb
        end
        Generator.new(erb).generate(*args)
      end

      def initialize(erb)
        @erb = erb
      end

      def generate(*args)
        target = args[0]
        @erb.result(binding)
      end

      private

      def template(sym, *args)
        self.class.generate sym, *args
      end

      def api_name(name)
        "B" + name
      end

    end
  end
end
