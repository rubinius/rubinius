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
        @to.bytecode(g)
        @from.bytecode(g)
        g.send :alias_method, 2, true
      end

      def to_sexp
        [:alias, @to.to_sexp, @from.to_sexp]
      end
    end

    class VAlias < Alias
      def bytecode(g)
        pos(g)

        g.push_rubinius
        g.find_const :Globals
        g.push_literal @from
        g.push_literal @to
        # TODO: fix #add_alias arg order to match #alias_method
        g.send :add_alias, 2
      end

      def to_sexp
        [:valias, @to, @from]
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
        @name.bytecode(g)
        g.send :__undef_method__, 1
      end

      def to_sexp
        [:undef, @name.to_sexp]
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

      def bytecode(g)
        count = @array.size - 1
        @array.each_with_index do |x, i|
          start_ip = g.ip
          x.bytecode(g)
          g.pop unless start_ip == g.ip or i == count
        end
      end

      def to_sexp
        @array.inject([:block]) { |s, x| s << x.to_sexp }
      end
    end

    class ClosedScope < Node
      include Compiler::LocalVariables

      attr_accessor :body

      # A nested scope is looking up a local variable. If the variable exists
      # in our local variables hash, return a nested reference to it.
      def search_local(name)
        if variable = variables[name]
          variable.nested_reference
        end
      end

      def new_local(name)
        variable = Compiler::LocalVariable.new allocate_slot
        variables[name] = variable
      end

      def new_nested_local(name)
        new_local(name).nested_reference
      end

      # There is no place above us that may contain a local variable. Set the
      # local in our local variables hash if not set. Set the local variable
      # node attribute to a reference to the local variable.
      def assign_local_reference(var)
        unless variable = variables[var.name]
          variable = new_local var.name
        end

        var.variable = variable.reference
      end

      def nest_scope(scope)
        scope.parent = self
      end

      def module?
        false
      end

      def attach_and_call(g, arg_name, scoped=false)
        name = @name || arg_name
        meth = new_generator(g, name)

        meth.push_state self

        if scoped
          meth.push_self
          meth.add_scope
        end

        meth.state.push_name name

        @body.bytecode meth

        meth.state.pop_name

        meth.ret
        meth.close

        meth.local_count = local_count
        meth.local_names = local_names

        meth.pop_state

        g.dup
        g.push_rubinius
        g.swap
        g.push_literal arg_name
        g.swap
        g.push_generator meth
        g.swap
        g.push_scope
        g.swap
        g.send :attach_method, 4
        g.pop
        g.send arg_name, 0

        return meth
      end

      def to_sexp
        sexp = [:scope]
        sexp << @body.to_sexp if @body
        sexp
      end
    end

    class Define < ClosedScope
      attr_accessor :name, :arguments

      def initialize(line, name, block)
        @line = line
        @name = name
        @arguments = block.strip_arguments
        block.array << NilLiteral.new(line) if block.array.empty?
        @body = block
      end

      def compile_body(g)
        meth = new_generator(g, @name, @arguments)

        meth.push_state self
        meth.state.push_super self
        pos(meth)

        meth.state.push_name @name

        @arguments.bytecode(meth)
        @body.bytecode(meth)

        meth.state.pop_name

        meth.local_count = local_count
        meth.local_names = local_names

        meth.ret
        meth.close
        meth.pop_state

        return meth
      end

      def bytecode(g)
        pos(g)

        g.push_rubinius
        g.push_literal @name
        g.push_generator compile_body(g)
        g.push_scope
        g.push_variables
        g.send :method_visibility, 0

        g.send :add_defn_method, 4
      end

      def to_sexp
        [:defn, @name, @arguments.to_sexp, [:scope, @body.to_sexp]]
      end
    end

    class DefineSingleton < Node
      attr_accessor :receiver, :body

      def initialize(line, receiver, name, block)
        @line = line
        @receiver = receiver
        @body = DefineSingletonScope.new line, name, block
      end

      def bytecode(g)
        pos(g)

        @body.bytecode(g, @receiver)
      end

      def to_sexp
        [:defs, @receiver.to_sexp, @body.name,
          @body.arguments.to_sexp, [:scope, @body.body.to_sexp]]
      end
    end

    class DefineSingletonScope < Define
      def initialize(line, name, block)
        super line, name, block
      end

      def bytecode(g, recv)
        pos(g)

        g.push_rubinius
        g.push_literal @name
        g.push_generator compile_body(g)
        g.push_scope
        recv.bytecode(g)

        g.send :attach_method, 4
      end
    end

    class FormalArguments < Node
      attr_accessor :names, :required, :optional, :defaults, :splat
      attr_reader :block_arg

      def initialize(line, args, defaults, splat)
        @line = line
        @defaults = nil
        @block_arg = nil

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

        if splat.kind_of? Symbol
          args << splat
        elsif splat
          splat = :@unnamed_splat
          args << splat
        end
        @names = args
        @splat = splat
      end

      def block_arg=(node)
        @names << node.name
        @block_arg = node
      end

      def bytecode(g)
        map_arguments g.state.scope

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
        if @splat
          index = @names.size
          index -= 1 if @block_arg
          index -= 1 if @splat.kind_of? Symbol
          index
        end
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

      def to_sexp
        sexp = [:args]

        @required.each { |x| sexp << x }
        sexp += @defaults.names if @defaults

        if @splat == :@unnamed_splat
          sexp << :*
        elsif @splat
          sexp << :"*#{@splat}"
        end

        sexp << :"&#{@block_arg.name}" if @block_arg

        sexp << [:block] + @defaults.to_sexp if @defaults

        sexp
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

      def to_sexp
        @arguments.map { |x| x.to_sexp }
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

        g.push_proc
        g.set_local @variable.slot
        g.pop
      end
    end

    class Class < Node
      attr_accessor :name, :superclass, :body

      def initialize(line, name, superclass, body)
        @line = line

        @superclass = superclass ? superclass : NilLiteral.new(line)

        case name
        when Symbol
          @name = ClassName.new line, name, @superclass
        when ToplevelConstant
          @name = ToplevelClassName.new line, name, @superclass
        else
          @name = ScopedClassName.new line, name, @superclass
        end

        if body
          @body = ClassScope.new line, @name, body
        else
          @body = EmptyBody.new line
        end
      end

      def bytecode(g)
        @name.bytecode(g)
        @body.bytecode(g)
      end

      def to_sexp
        superclass = @superclass.kind_of?(NilLiteral) ? nil : @superclass.to_sexp
        [:class, @name.to_sexp, superclass, @body.to_sexp]
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

      def bytecode(g)
        pos(g)

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
        g.push_rubinius
        g.push_literal @name
        @superclass.bytecode(g)
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_scope
        g.send :open_class, 3
      end

      def to_sexp
        @name
      end
    end

    class ToplevelClassName < ClassName
      def initialize(line, node, superclass)
        @line = line
        @name = node.name
        @superclass = superclass
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_cpath_top
        g.send :open_class_under, 3
      end

      def to_sexp
        [:colon3, @name]
      end
    end

    class ScopedClassName < ClassName
      attr_accessor :parent

      def initialize(line, node, superclass)
        @line = line
        @name = node.name
        @parent = node.parent
        @superclass = superclass
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        @parent.bytecode(g)
        g.send :open_class_under, 3
      end

      def to_sexp
        [:colon2, @parent.to_sexp, @name]
      end
    end

    class Module < Node
      attr_accessor :name, :body

      def initialize(line, name, body)
        @line = line

        case name
        when Symbol
          @name = ModuleName.new line, name
        when ToplevelConstant
          @name = ToplevelModuleName.new line, name
        else
          @name = ScopedModuleName.new line, name
        end

        if body
          @body = ModuleScope.new line, @name, body
        else
          @body = EmptyBody.new line
        end
      end

      def bytecode(g)
        @name.bytecode(g)
        @body.bytecode(g)
      end

      def to_sexp
        [:module, @name.to_sexp, @body.to_sexp]
      end
    end

    class EmptyBody < Node
      def bytecode(g)
        g.pop
        g.push :nil
      end

      def to_sexp
        [:scope]
      end
    end

    class ModuleName < Node
      attr_accessor :name

      def initialize(line, name)
        @line = line
        @name = name
      end

      def name_bytecode(g)
        g.push_rubinius
        g.push_literal @name
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_scope
        g.send :open_module, 2
      end

      def to_sexp
        @name
      end
    end

    class ToplevelModuleName < ModuleName
      def initialize(line, node)
        @line = line
        @name = node.name
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        g.push_cpath_top
        g.send :open_module_under, 2
      end

      def to_sexp
        [:colon3, @name]
      end
    end

    class ScopedModuleName < ModuleName
      attr_accessor :parent

      def initialize(line, node)
        @line = line
        @name = node.name
        @parent = node.parent
      end

      def bytecode(g)
        pos(g)

        name_bytecode(g)
        @parent.bytecode(g)
        g.send :open_module_under, 2
      end

      def to_sexp
        [:colon2, @parent.to_sexp, @name]
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

      def bytecode(g)
        pos(g)

        attach_and_call g, :__module_init__, true
      end
    end

    class SClass < Node
      attr_accessor :receiver

      def initialize(line, receiver, body)
        @line = line
        @receiver = receiver
        @body = SClassScope.new line, body
      end

      def bytecode(g)
        pos(g)
        @receiver.bytecode(g)
        @body.bytecode(g)
      end

      def to_sexp
        [:sclass, @receiver.to_sexp, @body.to_sexp]
      end
    end

    class SClassScope < ClosedScope
      def initialize(line, body)
        @line = line
        @body = body
        @name = nil
      end

      def bytecode(g)
        pos(g)

        g.push_rubinius
        g.swap
        g.send :object_metaclass, 1

        if @body
          # if @body just returns self, don't bother with it.
          if @body.kind_of? Block
            ary = @body.array
            return if ary.size == 1 and ary[0].kind_of? Self
          end

          # Ok, emit it.
          attach_and_call g, :__metaclass_init__, true
        else
          g.pop
          g.push :nil
        end
      end
    end

    class Container < ClosedScope
      attr_accessor :file, :name, :variable_scope

      def initialize(body)
        @body = body || NilLiteral.new(1)
      end

      def container_bytecode(g)
        g.name = @name
        g.file = @file.to_sym

        yield if block_given?

        g.local_count = local_count
        g.local_names = local_names
      end
    end

    class EvalExpression < Container
      def initialize(body)
        super body
        @name = :__eval_script__
      end

      def search_scopes(name)
        depth = 1
        scope = @variable_scope
        while scope
          if slot = scope.method.local_slot(name)
            return Compiler::NestedLocalVariable.new(depth, slot)
          elsif scope.eval_local_defined?(name, false)
            return Compiler::EvalLocalVariable.new(name)
          end

          depth += 1
          scope = scope.parent
        end
      end

      # Returns a cached reference to a variable or searches all
      # surrounding scopes for a variable. If no variable is found,
      # it returns nil and a nested scope will create the variable
      # in itself.
      def search_local(name)
        if variable = variables[name]
          return variable.nested_reference
        end

        if variable = search_scopes(name)
          variables[name] = variable
          return variable.nested_reference
        end
      end

      def new_local(name)
        variable = Compiler::EvalLocalVariable.new name
        @variable_scope.dynamic_locals[name] = nil
        variables[name] = variable
      end

      def assign_local_reference(var)
        unless reference = search_local(var.name)
          variable = new_local var.name
          reference = variable.reference
        end

        var.variable = reference
      end

      def bytecode(g)
        super(g)

        container_bytecode(g) do
          g.push_state self
          g.state.push_eval self
          @body.bytecode(g)
          g.ret
          g.pop_state
        end
      end

      def to_sexp
        [:eval, @body.to_sexp]
      end
    end

    class Snippet < Container
      def initialize(body)
        super body
        @name = :__snippet__
      end

      def bytecode(g)
        super(g)

        container_bytecode(g) do
          g.push_state self
          @body.bytecode(g)
          g.pop_state
        end
      end

      def to_sexp
        [:snippet, @body.to_sexp]
      end
    end

    class Script < Container
      def initialize(body)
        super body
        @name = :__script__
      end

      def bytecode(g)
        super(g)

        container_bytecode(g) do
          g.push_state self
          @body.bytecode(g)
          g.pop
          g.push :true
          g.ret
          g.pop_state
        end
      end

      def to_sexp
        [:script, @body.to_sexp]
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

        @expression.defined(g)
      end

      def to_sexp
        [:defined, @expression.to_sexp]
      end
    end
  end
end
