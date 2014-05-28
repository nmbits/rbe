
require 'erb'

module B
  module Gen
    module Util
      def self.template_dir=(dir)
        @@templates = dir
      end

      def self.create_template(filename)
        path = File.join @@templates, filename
        ERB.new File.read(path), nil, '%'
      end
    end
  end
end
