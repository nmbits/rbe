require 'rbe/cpp/class'
require 'rbe/cpp/function'
require 'rbe/cpp/interface'
require 'rbe/cpp/variable'
require 'rbe/cpp/type'

require 'erb'

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

    def initialize(io)
      @io = io
    end
    
    def parse
      mode = Mode.new
      buffer = ""
      io = @io
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
  end

  module Translator
    def self.translate(reader)
      answer = []
      clazz = nil
      access = :private
      reader.parse do |what, *args|
        case what
        when :class_start
          access = :private
          name, super_classes = args
          clazz = RBe::Cpp::Class.new name
          unless super_classes.empty?
            clazz.super_class_name = super_classes.first
          end
        when :public, :protected, :private
          access = what
        when :member
          proto = args.first
          if proto.include? "("
            ftype, name, interface = RBe::Cpp::Function.parse proto
            next unless ftype == :ctor || ftype == :function || ftype == :static
            f = clazz.function_by_name name
            unless f
              f = RBe::Cpp::Function.new name
              clazz.add_function f
            end
            f.add_interface interface
            case ftype
            when :ctor
              f.set_option :ctor
              f.set_option access
            when :function
              f.set_option access
            when :static
              f.set_option access
              f.set_option :static
            end
          else
            variable = RBe::Cpp::Variable.parse proto
            clazz.add_variable variable
          end
        when :class_end
          answer << clazz
          clazz = nil
        end
      end
      answer
    end
  end

  class Converter
    TEMPLATE = ERB.new <<'EOL', nil, '%'
% c = target
define_class "<%= c.name %>" do
  kit "<%= c.kit %>"
  header_file "<%= c.header_file %>"
% if c.super_class_name
  super_class_name "<%= c.super_class_name %>"
% end
% unless c.options.empty?
  options <%= c.options.keys.map{|k| ":#{k.to_s}"}.join ", " %>
% end
% c.variables.each do |v|
  variable "<%= v.to_ruby %>"
% end
% c.find_functions.each do |f|
  function "<%= f.name %>" do
% if !f.options.nil? && !f.options.empty?
    options <%= f.options.keys.map{|k| ":#{k.to_s}"}.join ", " %>
% end
%   f.interfaces.each do |i|
    interface <%= i.to_ruby %>
%     unless i.options.empty?
%     end
%   end
  end
% end
end
EOL
    def self.convert(clazz)
      target = clazz
      TEMPLATE.result(binding)
    end
  end
end

if __FILE__ == $0
  file_name = ARGV[0]
  class_name = ARGV[1]
  kit = File.dirname(file_name).split('/').last
  classes = nil
  File.open file_name do |file|
    reader = RBe::HeaderReader.new(file)
    classes = RBe::Translator.translate reader
  end
  classes.each do |target|
    if class_name == target.name
      target.kit = kit
      target.header_file = "#{kit}/#{File.basename(file_name)}"
      print RBe::Converter.convert(target)
    end
  end
end
