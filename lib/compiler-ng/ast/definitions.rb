module Rubinius
  module AST
    class Alias < Node
      attr_accessor :to, :from

      def initialize(line, to, from)
        @line = line
        @to = to
        @from = from
      end

      def bytecode(g)
        pos(g)

        g.push_scope
        g.push_literal @to
        g.push_literal @from
        g.send :alias_method, 2, true
      end
    end

    class VAlias < Alias
      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.find_const :Globals
        g.push_literal @from
        g.push_literal @to
        # TODO: fix #add_alias arg order to match #alias_method
        g.send :add_alias, 2
      end
    end

    class Undef < Node
      attr_accessor :name

      def initialize(line, sym)
        @line = line
        @name = sym
      end

      def bytecode(g)
        pos(g)

        g.push_scope
        g.push_literal @name
        g.send :__undef_method__, 1
      end
    end

    # Is it weird that Block has the :arguments attribute? Yes. Is it weird
    # that MRI parse tree puts arguments and block_arg in Block? Yes. So we
    # make do and pull them out here rather than having something else reach
    # inside of Block.
    class Block < Node
      attr_accessor :array

      def initialize(line, array)
        @line = line
        @array = array
      end

      def strip_arguments
        if @array.first.kind_of? FormalArguments
          node = @array.shift
          if @array.first.kind_of? BlockArgument
            node.block_arg = @array.shift
          end
          return node
        end
      end

      def children
        @array
      end

      def bytecode(g)
        # TODO: make plugins responsible for popping or not
        count = @array.size - 1
        i = 0
        while i < count
          ip = g.ip
          @array[i].bytecode(g)

          # guards for things that plugins might optimize away.
          g.pop if g.advanced_since?(ip)
          i += 1
        end
        @array[count].bytecode(g)
      end
    end

    class ClosedScope < Node
      attr_accessor :body

      def new_description(g)
        desc = Compiler::MethodDescription.new(g.class, nil)
        desc.generator.file = g.file
        desc
      end

      def children
        [@body]
      end

      def local_count
        variables.size
      end

      def local_names
        names = Array.new local_count
        variables.each_pair { |name, var| names[var.slot] = name }
        names
      end

      def variables
        @variables ||= {}
      end

      def allocate_slot
        variables.size
      end

      # A nested scope is looking up a local variable. If the variable exists
      # in our local variables hash, return a nested reference to it.
      def search_local(name)
        if variable = variables[name]
          CompilerNG::NestedLocalReference.new variable
        end
      end

      def new_local(name)
        variable = CompilerNG::LocalVariable.new allocate_slot
        variables[name] = variable
      end

      # There is no place above us that may contain a local variable. Set the
      # local in our local variables hash if not set. Set the local variable
      # node attribute to a reference to the local variable.
      def assign_local_reference(var)
        unless variable = variables[var.name]
          variable = new_local var.name
        end

        var.variable = CompilerNG::LocalReference.new variable
      end

      def map_variables
        visit self do |scope, node|
          result = scope
          case node
          when ClosedScope
            result = nil
          when ClassVariableAccess, ClassVariableAssignment
            node.in_module if scope.module?
          when LocalVariable
            scope.assign_local_reference node
          when FormalArguments
            node.map_arguments scope
          when Iter
            scope.nest_scope node
            result = node
          end

          result
        end
      end

      def nest_scope(scope)
        scope.parent = self
      end

      def module?
        false
      end

      def scope_bytecode(g)
        map_variables
      end

      def bytecode(g)
        pos(g)

        scope_bytecode(g)
      end

      def attach_and_call(g, name, scoped=false)
        desc = new_description(g)
        desc.name = name

        meth = desc.generator
        meth.name = @name ? @name : name

        meth.local_count = local_count
        meth.local_names = local_names

        if scoped
          meth.push_self
          meth.add_scope
        end

        @body.bytecode meth

        meth.ret
        meth.close

        g.dup
        g.push_const :Rubinius
        g.swap
        g.push_literal name
        g.swap
        g.push_literal desc
        g.swap
        g.push_scope
        g.swap
        g.send :attach_method, 4
        g.pop
        g.send name, 0

        return desc
      end
    end

    class Define < ClosedScope
      attr_accessor :name, :arguments

      def initialize(line, name, block)
        @line = line
        @name = name
        @arguments = block.strip_arguments
        block.array << Nil.new(line) if block.array.empty?
        @body = block
        map_super
      end

      def map_super
        visit do |result, node|
          case node
          when ClosedScope
            result = nil
          when ZSuper
            node.name = name
            node.arguments = arguments.to_actual node.line
            node.block = arguments.block_arg
          when Super
            node.name = name
          end

          result
        end
      end

      def compile_body(g)
        desc = new_description(g)
        meth = desc.generator
        meth.name = @name

        @arguments.bytecode(meth) if @arguments
        @body.bytecode(meth)

        meth.required_args = @arguments.required_args
        meth.total_args = @arguments.total_args
        meth.splat_index = @arguments.splat_index

        meth.local_count = local_count
        meth.local_names = local_names

        meth.ret
        meth.close

        return desc
      end

      def children
        [@arguments, @body]
      end

      def bytecode(g)
        super(g)

        g.push_const :Rubinius
        g.push_literal @name
        g.push_literal compile_body(g)
        g.push_scope

        if kernel?
          g.push :nil
        else
          g.push_variables
          g.send :method_visibility, 0
        end

        g.send :add_defn_method, 4
      end
    end

    class DefineSingleton < Define
      attr_accessor :receiver

      def initialize(line, receiver, name, block)
        @receiver = receiver
        super line, name, block
      end

      def bytecode(g)
        pos(g)

        scope_bytecode(g)

        if kernel?
          g.push_const :Rubinius
          g.push_literal @name
          g.push_literal compile_body(g)
          g.push_scope
          @receiver.bytecode(g)
          g.send :attach_method, 4
        else
          @receiver.bytecode(g)
          g.send :metaclass, 0
          g.push_literal @name
          g.push_literal compile_body(g)
          g.push_scope
          g.send :attach_method, 3
        end
      end

      def children
        [@receiver, @arguments, @body]
      end
    end

    class FormalArguments < Node
      attr_accessor :names, :required, :optional, :defaults, :splat
      attr_reader :block_arg

      def initialize(line, args, defaults, splat)
        @line = line

        if defaults
          defaults = DefaultArguments.new line, defaults
          @defaults = defaults
          @optional = defaults.names

          stop = defaults.names.first
          last = args.each_with_index { |a, i| break i if a == stop }
          @required = args[0, last]
        else
          @required = args.dup
          @optional = []
        end

        args << splat if splat.kind_of? Symbol
        @names = args
        @splat = splat
      end

      def block_arg=(node)
        @names << node.name
        @block_arg = node
      end

      def children
        [@defaults, @block_arg]
      end

      def bytecode(g)
        @defaults.bytecode(g) if @defaults
        @block_arg.bytecode(g) if @block_arg
      end

      def arity
        @required.size
      end

      def required_args
        @required.size
      end

      def total_args
        @required.size + @optional.size
      end

      def splat_index
        @names.size - 1 if @splat.kind_of? Symbol
      end

      def map_arguments(scope)
        @required.each { |arg| scope.new_local arg }
        @defaults.map_arguments scope if @defaults
        scope.new_local @splat if @splat.kind_of? Symbol
        scope.assign_local_reference @block_arg if @block_arg
      end

      def to_actual(line)
        arguments = ActualArguments.new line

        last = -1
        last -= 1 if @block_arg and @block_arg.name == names[last]
        last -= 1 if @splat == names[last]

        arguments.array = @names[0..last].map { |name| LocalVariableAccess.new line, name }

        if @splat.kind_of? Symbol
          arguments.splat = SplatValue.new(line, LocalVariableAccess.new(line, @splat))
        end

        arguments
      end
    end

    class DefaultArguments < Node
      attr_accessor :arguments, :names

      def initialize(line, block)
        @line = line
        array = block.array
        @names = array.map { |a| a.name }
        @arguments = array
      end

      def children
        @arguments
      end

      def map_arguments(scope)
        @arguments.each { |var| scope.assign_local_reference var }
      end

      def bytecode(g)
        @arguments.each do |arg|
          done = g.new_label

          g.passed_arg arg.variable.slot
          g.git done
          arg.bytecode(g)
          g.pop

          done.set!
        end
      end
    end

    module LocalVariable
      attr_accessor :variable
    end

    class BlockArgument < Node
      include LocalVariable

      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def bytecode(g)
        pos(g)

        g.push_block
        g.dup
        g.is_nil

        after = g.new_label
        g.git after

        g.push_const :Proc
        g.swap
        g.send :__from_block__, 1

        after.set!

        g.set_local @variable.slot
        g.pop
      end
    end

    class ClassWrapper < Node
      attr_accessor :name, :superclass, :body

      def initialize(line, name, superclass, body)
        @line = line

        @superclass = superclass ? superclass : Nil.new(line)

        if name.kind_of? Symbol
          @name = ClassName.new line, name, @superclass
        else
          @name = ScopedClassName.new line, name, @superclass
        end

        if body
          @body = ClassScope.new line, @name, body
        else
          @body = EmptyBody.new line
        end
      end

      def children
        [@name, @superclass, @body]
      end

      def bytecode(g)
        @name.bytecode(g)
        @body.bytecode(g)
      end
    end

    class ClassScope < ClosedScope
      def initialize(line, name, body)
        @line = line
        @name = name.name
        @body = body
      end

      def module?
        true
      end

      def children
        [@body]
      end

      def bytecode(g)
        super(g)

        attach_and_call g, :__class_init__, true
      end
    end

    class ClassName < Node
      attr_accessor :name, :superclass

      def initialize(line, name, superclass)
        @line = line
        @name = name
        @superclass = superclass
      end

      def name_bytecode(g)
        g.push_const :Rubinius
        g.push_literal @name
        @superclass.bytecode(g)
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_scope
        g.send :open_class, 3
      end

      def children
        [@superclass]
      end
    end

    class ScopedClassName < ClassName
      attr_accessor :parent

      def initialize(line, parent, superclass)
        @line = line
        @name = parent.name
        @parent = parent.parent
        @superclass = superclass
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        @parent.bytecode(g)
        g.send :open_class_under, 3
      end

      def children
        [@superclass, @parent]
      end
    end

    class ModuleWrapper < Node
      attr_accessor :name, :body

      def initialize(line, name, body)
        @line = line

        if name.kind_of? Symbol
          @name = ModuleName.new line, name
        else
          @name = ScopedModuleName.new line, name
        end

        if body
          @body = ModuleScope.new line, @name, body
        else
          @body = EmptyBody.new line
        end
      end

      def children
        [@name, @body]
      end

      def bytecode(g)
        @name.bytecode(g)
        @body.bytecode(g)
      end
    end

    class EmptyBody < Node
      def bytecode(g)
        g.pop
        g.push :nil
      end
    end

    class ModuleName < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def name_bytecode(g)
        g.push_const :Rubinius
        g.push_literal @name
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_scope
        g.send :open_module, 2
      end
    end

    class ScopedModuleName < ModuleName
      attr_accessor :parent

      def initialize(line, parent)
        @line = line
        @name = parent.name
        @parent = parent.parent
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        @parent.bytecode(g)
        g.send :open_module_under, 2
      end

      def children
        [@parent]
      end
    end

    class ModuleScope < ClosedScope
      def initialize(line, name, body)
        @line = line
        @name = name.name
        @body = body
      end

      def module?
        true
      end

      def children
        [@body]
      end

      def bytecode(g)
        super(g)

        attach_and_call g, :__module_init__, true
      end
    end

    class SClass < ClosedScope
      attr_accessor :receiver

      def initialize(line, receiver, body)
        @line = line
        @receiver = receiver
        @body = body
      end

      def children
        [@receiver, @body]
      end

      def bytecode(g)
        super(g)

        @receiver.bytecode(g)
        g.dup
        g.send :__verify_metaclass__, 0
        g.pop
        g.push_const :Rubinius
        g.swap
        g.send :open_metaclass, 1

        if @body
          attach_and_call g, :__metaclass_init__, true
        else
          g.pop
          g.push :nil
        end
      end
    end

    class Container < ClosedScope
      attr_accessor :file, :name

      def initialize(body)
        @body = body || Nil.new(1)
      end

      def bytecode(g)
        super(g)

        g.name = @name
        g.file = @file.to_sym
      end
    end

    class Snippit < Container
      def initialize(body)
        super body
        @name = :__snippit__
      end

      def bytecode(g)
        super(g)

        @body.bytecode(g)
      end
    end

    class Script < Container
      def initialize(body)
        super body
        @name = :__script__
      end

      def bytecode(g)
        super(g)

        @body.bytecode(g)
        g.pop
        g.push :true
        g.ret
      end
    end

    class Defined < Node
      attr_accessor :expression

      def initialize(line, expr)
        @line = line
        @expression = expr
      end

      def bytecode(g)
        pos(g)

        # TODO: implement as evaluator on @expression AST
      end
    end
  end
end
