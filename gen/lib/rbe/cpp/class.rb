
module RBe
  module Cpp
    class Class
      attr_accessor :name, :kit, :super_class_name, :class_list, :header_file, :mixin_name
      attr_reader :variables, :options

      OPTIONS = [:wrapper, :ancestors, :module, :custom_free, :custom_mark,
                 :hide_hooks]

      def initialize(name, super_class_name = nil, class_list = nil)
        @name = name
        @super_class_name = super_class_name
        @options = {}
        @variables = []
        @functions = []
        @class_list = class_list
      end

      def set_option(key, value = true)
        @options[key] = value
      end

      def option(key)
        @options[key]
      end        

      OPTIONS.each do |sym|
        define_method(sym.to_s + "?") { option sym }
      end

      def super_class
        raise "class_list is not specified" unless @class_list
        @class_list[@super_class_name]
      end

      def mixin
        raise "class_list is not specified" unless @class_list
        @class_list[@mixin_name]
      end

      def ancestors
        s = super_class
        return (s ? s.ancestors + [s] : [])
      end

      def root_class
        @root ||= (s = super_class) ? s.root_class : self
      end

      def dependencies
        names = {}
        @functions.each do |f|
          next if f.ignore? || f.custom?
          f.dependencies.each do |name|
            names[name] = true
          end
        end
        names[@super_class_name] = true if @super_class_name
        names.delete "Behavior" # TODO
        names.keys
      end

      def add_variable(v)
        @variables << v
      end

      def add_function(function)
        raise "function #{function.name} is already a member of a class" if function.clazz
        function.clazz = self
        @functions << function
      end

      def find_functions(condition = {})
        @functions.find_all {|f| f.match condition }
      end
      
      def function_by_name(name)
        @functions.find {|f| f.name == name }
      end

      def remove_function_by_name(name)
        @functions.delete_if {|f| f.name == name }
      end

      def hooks
        find_functions :hook => true
      end

      def ctor
        functions = find_functions :ctor => true
        if functions.empty?
          raise "#{@name} do not have ctor"
        else
          functions.first
        end
      end
      
      def super_hooks
        return [] unless super_class
        parent_hooks = super_class.hooks
        super_class.super_hooks.each do |hook|
          unless parent_hooks.find {|f| f.name == hook.name }
            parent_hooks.push hook
          end
        end
        parent_hooks.delete_if {|f| function_by_name f.name }
        parent_hooks
      end

      def binding_base_name
        @name.sub(/^B/, '')
      end

      def is_kind_of(class_name)
        return true if @name == class_name
        return true if @mixin_name == class_name
        if super_class
          return super_class.is_kind_of(class_name)
        end
        return false
      end
    end
  end
end
