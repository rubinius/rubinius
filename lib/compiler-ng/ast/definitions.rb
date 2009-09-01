module Rubinius
  module AST
    class Alias < Node
      attr_accessor :to, :from

      def self.from(p, to, from)
        node = Alias.new p.compiler
        node.to = to
        node.from = from
        node
      end

      def bytecode(g)
        g.push_scope
        g.push_literal @to
        g.push_literal @from
        g.send :alias_method, 2, true
      end
    end

    class VAlias < Node
      attr_accessor :to, :from

      def self.from(p, to, from)
        node = VAlias.new p.compiler
        node.to = to
        node.from = from
        node
      end

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

      def self.from(p, sym)
        literal = Literal.new(p.compiler).normalize(sym)
        node = Undef.new p.compiler
        node.name = literal.value
        node
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

      def self.from(p, array)
        node = Block.new p.compiler
        node.array = array
        node
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
      attr_accessor :locals

      def new_description
        Compiler::MethodDescription.new(@compiler.generator_class, self.locals)
      end

      def argument_info
        [0, 0, nil]
      end

      def children
        [@body]
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
          when CVar, CVarAssign
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
        scope_bytecode(g)
      end

      def attach_and_call(g, name, scoped=false)
        desc = new_description()
        desc.name = name
        meth = desc.generator

        # prelude(g, meth)

        if scoped
          meth.push_self
          meth.add_scope
        end

        desc.run self, @body

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
      attr_accessor :name, :arguments, :body

      def self.from(p, name, block)
        node = Define.new p.compiler
        node.name = name
        node.arguments = block.strip_arguments
        block.array << Nil.from(p) if block.array.empty?
        node.body = block
        node.map_super
        node
      end

      # TODO: fix
      def argument_info
        opt = @arguments.optional || [] # FIX: temporary

        [@arguments.arity, opt.size, @arguments.splat]
      end

      def map_super
        visit do |result, node|
          case node
          when ClosedScope
            result = nil
          when ZSuper
            node.name = name
            node.arguments = arguments.to_actual
            node.block = arguments.block_arg
          when Super
            node.name = name
          end

          result
        end
      end

      def compile_body(g)
        desc = new_description()
        meth = desc.generator

        # prelude(g, meth)

        @arguments.bytecode(meth) if @arguments
        desc.run self, @body # TODO: why is it not @body.bytecode(meth) ?

        # TODO: remove this, desc.args should have @arguments
        required = @arguments.required unless @arguments.required.empty?
        optional = @arguments.optional unless @arguments.optional.empty?
        desc.args = [required, optional, @arguments.splat]

        meth.ret
        meth.close

        # use_plugin g, :method, desc

        return desc
      end

      def children
        [@arguments, @body]
      end

      def bytecode(g)
        pos(g)

        scope_bytecode(g)

        g.push_const :Rubinius
        g.push_literal @name
        g.push_literal compile_body(g)
        g.push_scope

        if @compiler.kernel?
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

      def self.from(p, receiver, name, block)
        node = DefineSingleton.new p.compiler
        node.receiver = receiver
        node.name = name
        node.arguments = block.strip_arguments
        block.array << Nil.from(p) if block.array.empty?
        node.body = block
        node.map_super
        node
      end

      def bytecode(g)
        pos(g)

        scope_bytecode(g)

        if @compiler.kernel?
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

      def self.from(p, args, defaults, splat)
        node = FormalArguments.new p.compiler

        if defaults
          defaults = DefaultArguments.from p, defaults
          node.defaults = defaults
          node.optional = defaults.names

          stop = defaults.names.first
          last = args.each_with_index { |a, i| break i if a == stop }
          node.required = args[0, last]
        else
          node.required = args.dup
          node.optional = []
        end

        args << splat if splat.kind_of? Symbol
        node.names = args
        node.splat = splat

        node
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

      def map_arguments(scope)
        @required.each { |arg| scope.new_local arg }
        @defaults.map_arguments scope if @defaults
        scope.new_local @splat if @splat.kind_of? Symbol
        scope.assign_local_reference @block_arg if @block_arg
      end

      def to_actual
        arguments = ActualArguments.new @compiler

        last = -1
        last -= 1 if @block_arg and @block_arg.name == names[last]
        last -= 1 if @splat == names[last]

        locals = @names[0..last].map do |name|
          local = LocalAccess.new @compiler
          local.name = name
          local
        end
        arguments.array = locals

        if @splat.kind_of? Symbol
          value = LocalAccess.new @compiler
          value.name = @splat

          splat = SplatValue.new @compiler
          splat.value = value

          arguments.splat = splat
        end

        arguments
      end
    end

    class DefaultArguments < Node
      attr_accessor :arguments, :names

      def self.from(p, block)
        node = DefaultArguments.new p.compiler
        array = block.array
        node.names = array.map { |a| a.name }
        node.arguments = array
        node
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

    class LocalVariable < Node
      attr_accessor :name
    end

    class BlockArgument < LocalVariable
      attr_accessor :variable

      def self.from(p, name)
        node = BlockArgument.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
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

      def self.from(p, name, superclass, body)
        node = ClassWrapper.new p.compiler

        superclass = Nil.from p unless superclass
        node.superclass = superclass

        if name.kind_of? Symbol
          name = ClassName.from p, name, superclass
        else
          name = ScopedClassName.from p, name, superclass
        end
        node.name = name

        if body
          node.body = ClassScope.from p, name, body
        else
          node.body = EmptyBody.from p
        end

        node
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
      attr_accessor :name, :body

      def self.from(p, name, body)
        node = ClassScope.new p.compiler
        node.name = name
        node.body = body
        node
      end

      def module?
        true
      end

      def children
        [@body]
      end

      def bytecode(g)
        pos(g)
        super(g)

        desc = attach_and_call g, :__class_init__, true
        desc.name = @name if desc
      end
    end

    class ClassName < Node
      attr_accessor :name, :superclass

      def self.from(p, name, superclass)
        node = ClassName.new p.compiler
        node.name = name
        node.superclass = superclass
        node
      end

      def name_bytecode(g)
        g.push_const :Rubinius
        g.push_literal @name
        @superclass.bytecode(g)
      end

      def bytecode(g)
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

      def self.from(p, parent, superclass)
        node = ScopedClassName.new p.compiler
        node.name = parent.name
        node.parent = parent.parent
        node.superclass = superclass
        node
      end

      def bytecode(g)
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

      def self.from(p, name, body)
        node = ModuleWrapper.new p.compiler

        if name.kind_of? Symbol
          name = ModuleName.from p, name
        else
          name = ScopedModuleName.from p, name
        end
        node.name = name

        if body
          node.body = ModuleScope.from p, name, body
        else
          node.body = EmptyBody.from p
        end

        node
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
      def self.from(p)
        EmptyBody.new p.compiler
      end

      def bytecode(g)
        g.pop
        g.push :nil
      end
    end

    class ModuleName < Node
      attr_accessor :name

      def self.from(p, name)
        node = ModuleName.new p.compiler
        node.name = name
        node
      end

      def name_bytecode(g)
        g.push_const :Rubinius
        g.push_literal @name
      end

      def bytecode(g)
        name_bytecode(g)
        g.push_scope
        g.send :open_module, 2
      end
    end

    class ScopedModuleName < ModuleName
      attr_accessor :parent

      def self.from(p, parent)
        node = ScopedModuleName.new p.compiler
        node.name = parent.name
        node.parent = parent.parent
        node
      end

      def bytecode(g)
        name_bytecode(g)
        @parent.bytecode(g)
        g.send :open_module_under, 2
      end

      def children
        [@parent]
      end
    end

    class ModuleScope < ClosedScope
      attr_accessor :name, :body

      def self.from(p, name, body)
        node = ModuleScope.new p.compiler
        node.name = name
        node.body = body
        node
      end

      def module?
        true
      end

      def children
        [@body]
      end

      def bytecode(g)
        pos(g)
        super(g)

        attach_and_call g, :__module_init__, true
      end
    end

    class SClass < ClosedScope
      attr_accessor :receiver, :name, :body

      def self.from(p, receiver, body)
        node = SClass.new p.compiler
        node.receiver = receiver
        node.body = body
        node
      end

      def bytecode(g)
        pos(g)

        @receiver.bytecode(g)
        g.dup
        g.send :__verify_metaclass__, 0
        g.pop
        g.push_const :Rubinius
        g.swap
        g.send :open_metaclass, 1

        attach_and_call g, :__metaclass_init__, true
      end
    end

    class Scope < Node
      attr_accessor :body

      def self.from(p, body)
        if body.kind_of? Block
          body
        elsif body
          Block.from p, [body]
        end
      end
    end

    class Snippit < ClosedScope
      attr_accessor :body

      def self.from(compiler, body)
        node = Snippit.new compiler
        node.body = body
        node
      end

      def args(body)
        @body = expand body
      end

      def bytecode(g)
        super(g)
        @body.bytecode(g)

        #set(:scope, self) do
        #  prelude(nil, g)
        #  @body.bytecode(g)
        #end
      end
    end

    class Script < ClosedScope
      attr_accessor :body

      def self.from(p, body)
        node = Script.new p.compiler
        node.body = body
        node
      end

      def bytecode(g)
        super(g)

        prelude(nil, g)
        @body.bytecode(g)
        g.pop
        g.push :true
        g.ret
      end
    end

    class Defined < Node
      attr_accessor :expression

      def self.from(p, expr)
        node = Defined.new p.compiler
        node.expression = expr
        node
      end

      def bytecode(g)
        pos(g)

        # TODO: implement as evaluator on @expression AST
      end
    end
  end
end
