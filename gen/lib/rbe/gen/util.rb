
module RBe
  module Gen
    module Util
      def header_file_identifier(name)
        "RBE_#{underscore(name).upcase}_HPP"
      end

      def underscore(str)
        answer = str.dup
        answer.gsub! /([A-Z]+)([A-Z][a-z\d])/, '\1_\2'
        answer.gsub! /([a-z\d])([A-Z])/, '\1_\2'
        answer.downcase!
        answer
      end

      def class_binding_header(name)
        "#{class_binding_name(name)}.hpp"
      end

      def class_binding_name(class_name)
        class_name.sub(/^B/, '')
      end

      def hookst_proto(interface, for_impl = false)
        ret = interface.ret || 'void'
        name = interface.function.name
        clazz = interface.function.clazz
        if for_impl
          answer = "#{ret} #{class_binding_name clazz.name}::#{name}ST(#{clazz.name} *_this"
        else
          answer = "static #{ret} #{name}ST(#{clazz.name} *_this"
        end
        unless interface.args.empty?
          xargs = args_proto_string(interface.args)
          answer += ", " + xargs
        end
        answer += ")"
        answer
      end

      def args_proto_string(args)
        args.each_with_index.map {|a, i| "#{a.type} #{a.name}"}.join(", ")
      end

      def args_string(args)
        args.each_with_index.map{|a, i| "#{a.name}"}.join(", ")
      end

      def hookst_call(interface)
        function = interface.function
        clazz = function.clazz
        ret = interface.ret? ? "return " : ""
        arg_names = interface.args.map {|a| a.name }
        args = (["this"] + arg_names).join(", ")
        "#{ret}#{class_binding_name clazz.name}::#{function.name}ST(#{args})"
      end

      def inout_type(interface)
        i = 0
        xtypes_ary = []
        while i < interface.args.length
          a = interface.args[i]
          if a.out?
            xtypes_ary << "Out<#{a.type}>"
          elsif a.pair?
            b = interface.args[i+1]
            i += 1
            xtypes_ary << "In<std::pair<#{a.type}, #{b.type}> >"
          else
            xtypes_ary << "In<#{a.type}>"
          end
          i += 1          
        end
        xtypes_ary.empty? ? "void" : "InOut<" + xtypes_ary.join(", ") + " >"
      end

      def interface_type(interface)
        # xtypes_ary = interface.args.map do |a|
        #   if a.out?
        #     "Out<#{a.type}>"
        #   else
        #     "In<#{a.type}>"
        #   end
        # end
        answer = nil
        xtypes = inout_type interface
        if xtypes != 'void' || interface.ret?
          ret_type = interface.ret || 'void'
          answer = "Interface<#{xtypes}, #{ret_type} >"
        end
        answer
      end

      def inout_args(interface)
        if interface.arity > 0
          "(" + input_args_no_paren(interface) + ")"
        else
          ""
        end
      end

      def input_args_no_paren(interface)
        interface.args.map{|a| a.name }.join(', ')
      end

      def owner_name(owner, target_name)
        if owner.is_kind_of "BInvoker"
          "BInvoker"
        else
          owner.name
        end
      end
    end
  end
end
