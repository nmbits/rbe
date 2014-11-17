
module RBe
  class HeaderReader
    class Mode
      def initialize
        @stack = [:header]
      end

      def push(mode)
        @stack.push mode
      end

      def pop
        @stack.pop
      end

      def public?
        @stack.last == :public
      end

      def protected?
        @stack.last == :protected
      end

      def private?
        @stack.last == :private
      end

      def enum?
        @stack.last == :enum
      end

      def access_mode_expected?
        [:public, :protected, :private].include? @stack.last
      end

      def line_part_expected?
        access_mode_expected? || @stack.last == :line_part
      end

      def line_part?
        @stack.last == :line_part
      end

      def comment?
        @stack.last == :comment
      end

      def header?
        @stack.last == :header
      end

      def check_or_error(sym, line = "")
        raise "parse error? near #{line}, #{@stack}" unless self.__send__(sym)
      end
    end

    def self.parse(io)
      mode = Mode.new
      buffer = ""
      while orig = io.gets
        # remove comments
        line = orig.gsub(/\/\*.*\*\//, '').gsub(/\/\/.+/, '').strip
        next if /^\s*$/ =~ line

        case line
        when /\/\*/		# comment start
          raise "parse error ?" if mode.comment?
          mode.push :comment

        when /\*\//		# comment end
          raise "parse error near #{orig}" unless mode.comment?
          mode.pop
          next
        end

        next if mode.comment?

        line.gsub! /\t+/, " "

        case line

        when /^\s*(enum)\s+[A-Za-z0-9_]+\s*\{/
          mode.push :enum
          
        when /^\s*(class|struct)\s+[A-Za-z0-9_]+\s*;$/ # class reference. ignore.

        when /^\s*class\s+(B[A-Za-z0-9]+)\s*:?([^\{]+)?\s*\{/ # class start
          mode.check_or_error :header?, line
          clazz = $1
          mode.push :class
          mode.push :private
          if $2
            super_classes = $2.split(",").map{|c| c.gsub(/(public|protected|private)/, "").strip}
          else
            super_classes = []
          end
          yield :class_start, clazz, super_classes

        when /^(public|protected|private):$/ # public: or protected: or private:
          if mode.access_mode_expected?
            mode.pop
            mode.push $1.to_sym
            yield $1.to_sym
          end

        when /\{[^\}]+\}\s*$/   # maybe one line function
          next unless mode.public? || mode.protected?
          yield :member, $`

        when /[^\;]+$/	# lines which don't end with ";"
          next unless mode.public? || mode.protected?
          mode.check_or_error :line_part_expected?, line
          buffer += line.strip + " "
          
        when /^\s*\}\s*;\s*$/	# class end
          if mode.enum?
            mode.pop
            next
          end
          next unless mode.access_mode_expected?
          mode.pop
          mode.pop
          yield :class_end, $1

        when /\;\s*$/		# lines end with ";"
          next unless mode.public? || mode.protected?
          mode.pop if mode.line_part?
          yield :member, buffer + line.strip
          buffer = ""

          # otherwise ignored
        end
      end
    end

    private

    def parse_line(line)
      puts "\t#{line}"
      # case line
      # when /^\s*(virtual|static)?\s+([^\s]+)[A-Za-z0-9]+\s*\([^\)]*\)\s*(const)?/
      # end
    end
  end
end

if __FILE__ == $0
  clazz = ARGV[0]
  kit = ARGV[1]
  path = File.join("/home/nmbits/work/haiku/headers/os", kit, clazz)
  File.open path do |file|
    reader = RBe::HeaderReader.new clazz, file
    reader.parse do |s, *a|
      puts "#{s}, #{a}"
    end
  end
end
