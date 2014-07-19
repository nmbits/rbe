
module RBe
  module Cpp
    class Class
      attr_accessor :name, :ctors, :kit, :haiku_header

      @@registory = {}

      def self.[](name)
        @@registory[name]
      end

      def self.all_classes
        @@registory.values
      end

      def self.delete_class(name)
        @@registory.delete name
      end

      def initialize(name)
        @name = name
        @variables = []
        @inner_variables = []
        @functions = {}
        @static_functions = {}
        @ctors = []
        @@registory[name] = self
      end

      def super_class=(name)
        @super_class = name
      end

      def super_class
        @@registory[@super_class]
      end

      def ancestors
        s = super_class
        return (s ? s.ancestors + [s] : [])
      end

      def root_class
        unless @root
          s = super_class
          @root = s.nil? ? self : s.root_class
        end
        @root
      end

      def wrapper?
        @wrapper
      end

      def wrapper=(b)
        @wrapper = b
      end

      def module=(b)
        @module = b
      end

      def module?
        @module
      end

      def custom_free=(b)
        @custom_free = b
      end

      def custom_free?
        @custom_free
      end

      def references
        unless @references
          names = Hash.new
          function_names.each do |name|
            functions(name).each do |f|
              next if f.noimp? || f.custom?
              if /B([A-Z][a-zA-Z]+)/ =~ f.ret
                names[$1] = true
              end
              f.args.each do |a|
                if /B([A-Z][a-zA-Z]+)/ =~ a.type
                  names[$1] = true
                end
              end
            end
          end
          @references = names.keys
        end
        @references
      end

      def add_variable(v)
        @variables.push v
      end

      def variables
        @variables
      end

      def add_inner_variable(v)
        @inner_variables.push v
      end

      def inner_variables
        @inner_variables
      end

      def add_ctor(function)
        function.index = @ctors.size
        function.clazz = self
        @ctors.push function
      end

      def add_function(function, access)
        add_function_common(function, access, @functions)
      end

      def add_static_function(function, access)
        add_function_common(function, access, @static_functions)
      end

      def add_function_common(function, access, hash)
        list = (hash[function.name] ||= [access])
        function.index = list.size
        function.clazz = self
        list.push function
      end
      private :add_function_common

      def function_names(*accesses)
        a = []
        @functions.each do |k, v|
          a.push k if accesses.empty? || accesses.include?(v[0])
        end
        a
      end

      def set_hook(name)
        list = @functions[name]
        list[0] = :hook if list
      end

      def functions(name)
        list = @functions[name]
        ret = list[1..list.size-1]
        ret
      end

      def remove_functions(name)
        @functions.delete name
      end

      def hooks
        function_names(:hook).map{|name| @functions[name][1]}
      end

      def super_hooks
        ret = {}
        clazz = super_class
        hook_names = hooks.map{|f| f.name}
        while clazz != nil
          clazz.hooks.each do |hook|
            if !ret.include?(hook.name) && !hook_names.include?(hook.name)
              ret[hook.name] = hook
            end
          end
          clazz = clazz.super_class
        end
        ret.values
      end

      # for code generation

      def header_file
        "#{kit}/#{name}.h"
      end

      def api_name
        "B#{name}"
      end
    end
  end
end
