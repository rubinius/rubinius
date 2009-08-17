# Temporary location for modifications needed for any Compiler
# Node classes while getting Melbourne running.

# Use a different matcher while converting the parser/compiler
require 'spec/custom/helpers/generator'

class CompileAsMatcher
  def matches?(actual)
    compiler = Compiler.new TestGenerator
    # TODO: Fix the compiler to have a proper interface for
    # enabling plugins. All compiler specs should be written
    # without plugins enabled, and each plugin should have
    # specs for bytecode with and without the plugin enabled.
    compiler.instance_variable_get(:@plugins).clear
    @plugins.each { |plugin| compiler.activate plugin }

    node = Compiler::Node::Snippit.from compiler, actual.to_ast

    generator = TestGenerator.new
    node.bytecode generator

    @actual = generator
    @actual == @expected
  end
end

class Compiler
  # Temporary
  class LocalVar
    attr_reader :slot

    def initialize(slot)
      @slot = slot
    end
  end

  class LocalReference
    attr_reader :slot

    def initialize(var)
      @slot = var.slot
    end

    def get_bytecode(g)
      g.push_local @slot
    end

    def set_bytecode(g)
      g.set_local @slot
    end
  end

  class NestedLocalReference
    attr_accessor :depth
    attr_reader :slot

    def initialize(var)
      @slot = var.slot
      @depth = 0
    end

    def get_bytecode(g)
      g.push_local_depth @depth, @slot
    end

    def set_bytecode(g)
      g.set_local_depth @depth, @slot
    end
  end

  class EvalLocalReference
    def get_bytcode(g)
    end

    def set_bytecode(g)
    end
  end

  class AsciiGrapher
    def initialize(ast)
      @ast = ast
    end

    def print
      graph_node @ast
    end

    def indented_print(level, value)
      puts "#{" " * level}#{value}"
    end

    def print_node(node, level)
      name = node.class.to_s.split("::").last
      indented_print level, name
    end

    def graph_node(node, level=0)
      print_node node, level
      level += 2

      nodes = []
      node.instance_variables.each do |v|
        next if v == "@compiler"

        value = node.instance_variable_get v

        # lame, yes. remove when Node doesn't have @body by default
        next if v == "@body" and value.nil? and not v.respond_to? :body=

        if value.kind_of? Node
          nodes << value
        else
          graph_value v, value, level
        end
      end

      nodes.each { |n| graph_node n, level }
    end

    def graph_simple(name, value, level)
      puts "#{" " * level}#{name}: #{value}"
    end

    def graph_value(name, value, level)
      case value
      when NilClass, String
        graph_simple name, value.inspect, level
      when TrueClass, FalseClass, Symbol, Fixnum
        graph_simple name, value, level
      when Array
        puts "#{" " * level}#{name}:"
        nodes = []
        value.each do |v|
          if v.kind_of? Node
            nodes << v
          else
            graph_value "-", v, level + 2
          end
        end

        nodes.each { |n| graph_node n, level + 2 }
      else
        graph_simple name, value.class, level
      end
    end
  end


  class Node
    attr_accessor :parent

    def find_parent(klass, ceiling=nil)
      return unless @parent

      if @parent.is_a? klass
        @parent unless ceiling and @parent.is_a? ceiling
      else
        @parent.find_parent klass
      end
    end

    def children
      []
    end

    def bytecode(g)
    end

    def in_rescue
    end

    def visit(arg=true, &block)
      children.each do |child|
        if child
          next unless ch_arg = block.call(arg, child)
          child.visit(ch_arg, &block)
        end
      end
    end

    def ascii_graph
      AsciiGrapher.new(self).print
    end


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

    class And < Node
      def self.from(p, left, right)
        node = And.new p.compiler
        node.left = left
        node.right = right
        node
      end

      def bytecode(g, use_gif=true)
        @left.bytecode(g)
        g.dup
        lbl = g.new_label

        if use_gif
          g.gif lbl
        else
          g.git lbl
        end

        g.pop
        @right.bytecode(g)
        lbl.set!
      end
    end

    class ArgList < ArrayLiteral
      def self.from(p, normal, splat)
        node = ArgList.new p.compiler
        normal.body << Splat.from(p, splat)
        node.body = normal
        node
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
          node.required = args
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

    class ArrayLiteral < Node
      def self.from(p, array)
        node = ArrayLiteral.new p.compiler
        node.body = array
        node
      end

      def bytecode(g)
        @body.each do |x|
          x.bytecode(g)
        end

        g.make_array @body.size
      end
    end

    class AttrAssign < Call
      attr_accessor :receiver, :name, :arguments, :value

      def self.from(p, receiver, name, value)
        node = AttrAssign.new p.compiler
        node.receiver = receiver
        node.name = name
        if value.kind_of? ArrayLiteral
          node.value = value.body.pop
          node.arguments = value
        else
          node.value = value
        end
        node
      end

      def bytecode(g)
      end
    end

    class BackRef < Node
      def self.from(p, ref)
        node = BackRef.new p.compiler
        node.kind = ref
        node
      end

      def bytecode(g)
        g.push_variables
        g.push_literal @kind
        g.send :back_ref, 1
      end
    end

    class Begin < Node
      attr_accessor :rescue

      def self.from(p, body)
        node = Begin.new p.compiler
        node.rescue = body
        node
      end

      def children
        [@rescue]
      end

      def bytecode(g)
        @rescue.bytecode(g)
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

    class BlockArgument < LocalVariable
      attr_accessor :name, :variable

      def self.from(p, name)
        node = BlockArgument.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        g.set_local @variable.slot
      end
    end

    class BlockPass < Node
      attr_accessor :block

      def self.from(p, block)
        node = BlockPass.new p.compiler
        node.block = block
        node
      end

      def children
        [@block]
      end

      def bytecode(g)
        nil_block = g.new_label

        @block.bytecode(g)

        g.dup
        g.is_nil
        g.git nil_block

        g.push_cpath_top
        g.find_const :Proc

        g.swap
        g.send :__from_block__, 1

        nil_block.set!
      end
    end

    class Break < Node
      attr_accessor :value

      def self.from(p, expr)
        node = Break.new p.compiler
        node.value = expr || Nil.from(p)
        node
      end

      def bytecode(g)
        @value.bytecode(g)

        g.pop_unwind if @pop_unwind

        if g.break
          g.goto g.break
        elsif @in_block
          g.raise_break
        else
          g.pop
          g.push_const :Compiler
          g.find_const :Utils
          g.send :__unexpected_break__, 0
        end
      end
    end

    class Send < Node
      attr_accessor :receiver, :name, :privately, :block

      def self.from(p, receiver, name, privately=true)
        node = Send.new p.compiler
        node.receiver = receiver
        node.name = name
        node.privately = privately
        node
      end

      def children
        [@receiver, @block]
      end

      def bytecode(g)
        pos(g)

        # TODO: involve plugins

        @receiver.bytecode(g)

        if @block
          @block.bytecode(g)
          g.send_with_block @name, 0, @privately
        else
          g.send @name, 0, @privately
        end
      end
    end

    class SendWithArguments < Send
      attr_accessor :arguments

      def self.from(p, receiver, name, arguments, privately=true)
        node = SendWithArguments.new p.compiler
        node.receiver = receiver
        node.name = name
        node.arguments = ActualArguments.from p, arguments
        node.privately = privately
        node
      end

      def children
        [@receiver, @arguments, @block]
      end

      def bytecode(g)
        pos(g)

        # TODO: involve plugins

        @receiver.bytecode(g)
        @arguments.bytecode(g)

        if @arguments.splat?
          @block ? @block.bytecode(g) : g.push(:nil)
          g.send_with_splat @name, @arguments.size, @privately, false
        elsif @block
          @block.bytecode(g)
          g.send_with_block @name, @arguments.size, @privately
        else
          g.send @name, @arguments.size, @privately
        end
      end
    end

    # TODO: rename to Splat and fix Splat superclass
    class SplatValue < Node
      attr_accessor :value

      def self.from(p, value)
        node = SplatValue.new p.compiler
        node.value = value
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        @value.bytecode(g)
        g.cast_array unless @value.kind_of? ArrayLiteral
      end
    end

    class ActualArguments < Node
      attr_accessor :array, :splat

      def self.from(p, arguments)
        node = ActualArguments.new p.compiler
        if arguments.kind_of? SplatValue
          node.splat = arguments
          node.array = []
        elsif arguments.kind_of? ConcatArgs
          node.array = arguments.array.body
          node.splat = SplatValue.from p, arguments.rest
        else
          node.array = arguments.body
        end
        node
      end

      def size
        @array.size
      end

      def splat?
        not @splat.nil?
      end

      def children
        children = @array.dup
        children << @splat if splat?
        children
      end

      def bytecode(g)
        @array.each { |x| x.bytecode(g) }
        @splat.bytecode(g) if @splat
      end
    end

    class Case < Node
      attr_accessor :whens, :else

      def self.from(p, whens, else_body)
        node = Case.new p.compiler
        node.whens = whens
        node.else = else_body || Nil.from(p)
        node
      end

      def children
        children = @whens.dup
        children << @else
        children
      end

      def bytecode(g)
        done = g.new_label

        @whens.each do |w|
          nxt = g.new_label
          w.bytecode(g, false, nxt, done)
          nxt.set!
        end

        @else.bytecode(g)

        done.set!
      end
    end

    class ReceiverCase < Case
      attr_accessor :receiver

      def self.from(p, receiver, whens, else_body)
        node = ReceiverCase.new p.compiler
        node.receiver = receiver
        node.whens = whens
        node.else = else_body || Nil.from(p)
        node
      end

      def children
        children = [@receiver]
        children.concat @whens
        children << @else
        children
      end

      def bytecode(g)
        done = g.new_label

        @receiver.bytecode(g)

        @whens.each do |w|
          nxt = g.new_label
          w.bytecode(g, true, nxt, done)
          nxt.set!
        end

        g.pop

        @else.bytecode(g)

        done.set!
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

    class ClosedScope < Node
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
          NestedLocalReference.new variable
        end
      end

      def new_local(name)
        variable = LocalVar.new allocate_slot
        variables[name] = variable
      end

      # There is no place above us that may contain a local variable. Set the
      # local in our local variables hash if not set. Set the local variable
      # node attribute to a reference to the local variable.
      def assign_local_reference(var)
        unless variable = variables[var.name]
          variable = new_local var.name
        end

        var.variable = LocalReference.new variable
      end

      def map_locals
        visit self do |scope, node|
          result = scope
          case node
          when ClosedScope
            result = nil
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

      def scope_bytecode(g)
        map_locals
      end

      def bytecode(g)
        scope_bytecode(g)
      end

      def attach_and_call(g, name, scoped=false)
        desc = new_description()
        desc.name = name
        meth = desc.generator

        prelude(g, meth)

        if scoped
          meth.push_self
          meth.add_scope
        end

        set(:scope, self) do
          show_errors(meth) do
            desc.run self, @body
          end
        end

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

    class ConcatArgs < DynamicArguments
      def self.from(p, array, rest)
        node = ConcatArgs.new p.compiler
        node.array = array
        node.rest = rest
        node
      end

      def bytecode(g)
        @array.bytecode(g)
        @rest.bytecode(g)
        g.cast_array
        g.send :+, 1
      end
    end

    class ConstAccess < Node
      def self.from(p, parent, name)
        node = ConstAccess.new p.compiler
        node.parent = parent
        node.name = name
        node
      end

      def children
        [@parent]
      end

      def bytecode(g)
        pos(g)

        @parent.bytecode(g)
        g.find_const @name
      end
    end

    class ConstAtTop < Node
      attr_accessor :parent

      def self.from(p, name)
        node = ConstAtTop.new p.compiler
        node.name = name
        node.parent = TopLevel.from p
        node
      end

      def bytecode(g)
        pos(g)

        g.push_cpath_top
        g.find_const @name
      end
    end

    class TopLevel < Node
      def self.from(p)
        TopLevel.new p.compiler
      end

      def bytecode(g)
        g.push_cpath_top
      end
    end

    class ConstFind < Node
      def self.from(p, name)
        node = ConstFind.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        pos(g)
        g.push_const @name
      end
    end

    class ConstSet < Node
      def self.from(p, expr, value)
        node = ConstSet.new p.compiler
        node.name = expr
        node.value = value
        node
      end

      def bytecode(g)
      end
    end

    class CVar < Node
      def self.from(p, name)
        node = CVar.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
      end
    end

    class CVarAssign < Node
      def self.from(p, name, expr)
        node = CVarAssign.new p.compiler
        node.name = name
        node.value = expr
        node
      end

      def bytecode(g)
      end
    end

    class CVarDeclare < CVarAssign
      def self.from(p, name, expr)
        node = CVarDeclare.new p.compiler
        node.name = name
        node.value = expr
        node
      end

      def bytecode(g)
      end
    end

    class Define < ClosedScope
      attr_accessor :body

      def self.from(p, name, block)
        node = Define.new p.compiler
        node.name = name
        node.arguments = block.strip_arguments
        block.array << Nil.from(p) if block.array.empty?
        node.body = block
        node
      end

      def compile_body(g)
        desc = new_description()
        meth = desc.generator

        prelude(g, meth)

        set(:scope, nil) do
          show_errors(meth) do
            @arguments.bytecode(meth) if @arguments
            desc.run self, @body # TODO: why is it not @body.bytecode(meth) ?
          end
        end

        # TODO: remove this, desc.args should have @arguments
        required = @arguments.required unless @arguments.required.empty?
        optional = @arguments.optional unless @arguments.optional.empty?
        desc.args = [required, optional, @arguments.splat]

        meth.ret
        meth.close

        use_plugin g, :method, desc

        return desc
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

      def children
        [@body, @arguments]
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

    class DefineSingleton < Define
      attr_accessor :receiver

      def self.from(p, receiver, name, block)
        node = DefineSingleton.new p.compiler
        node.receiver = receiver
        node.name = name
        node.arguments = block.strip_arguments
        block.array << Nil.from(p) if block.array.empty?
        node.body = block
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

    class DynamicExecuteString < DynamicString
      def self.from(p, str, array)
        node = DynamicExecuteString.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true #`
      end
    end

    class DynamicRegex < DynamicString
      def self.from(p, str, array, flags)
        node = DynamicRegex.new p.compiler
        node.string = str
        node.array = array
        node.options = flags
        node
      end

      def bytecode(g)
        pos(g)

        g.push_const :Regexp
        super(g)
        g.push @options
        g.send :new, 2
      end
    end

    class DynamicOnceRegex < DynamicRegex
      def self.from(p, str, array, flags)
        node = DynamicRegex.new p.compiler
        node.string = str
        node.array = array
        node.options = flags
        node
      end

      def bytecode(g)
        pos(g)

        idx = g.add_literal(nil)
        g.push_literal_at idx
        g.dup
        g.is_nil

        lbl = g.new_label
        g.gif lbl
        g.pop

        super(g)

        g.set_literal idx
        lbl.set!
      end
    end

    class DynamicString < StringLiteral
      attr_accessor :array, :options

      def self.from(p, str, array)
        node = DynamicString.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def children
        @array
      end

      def bytecode(g)
        pos(g)

        @array.reverse_each do |x|
          x.bytecode(g)
        end
        g.push_literal @string
        g.string_dup

        @array.size.times do
          g.string_append
        end
      end
    end

    class DynamicSymbol < DynamicString
      def self.from(p, str, array)
        node = DynamicSymbol.new p.compiler
        node.string = str
        node.array = array
        node
      end

      def bytecode(g)
        pos(g)

        super(g)
        g.send :to_sym, 0, true
      end
    end

    class EmptyArray < Node
      def self.from(p)
        EmptyArray.new p.compiler
      end

      def bytecode(g)
        pos(g)

        g.make_array 0
      end
    end

    class Ensure < Node
      def self.from(p, body, ensr)
        node = Ensure.new p.compiler
        node.body = body || Nil.from(p)
        node.ensure = ensr
        node
      end

      def children
        [@body, @ensure]
      end

      def bytecode(g)
        pos(g)

        ok = g.new_label
        g.exceptions :ensure do |ex|
          @body.bytecode(g)
          ex.escape ok

          ex.handle!
          g.push_exception

          @ensure.bytecode(g)
          g.pop

          g.pop_exception

          # Re-raise the exception
          g.reraise
        end

        ok.set!

        # Now, re-emit the code for the ensure which will run if there was no
        # exception generated.
        @ensure.bytecode(g)
        g.pop
      end
    end

    class ExecuteString < StringLiteral
      def self.from(p, str)
        node = ExecuteString.new p.compiler
        node.string = str
        node
      end

      def bytecode(g)
        pos(g)

        g.push :self
        super(g)
        g.send :`, 1, true # ` (silly vim/emacs)
      end
    end

    class False < Node
      def self.from(p)
        False.new p.compiler
      end

      def bytecode(g)
        g.push :false
      end
    end

    class Flip2 < Node
      def self.from(p, start, finish)
        Flip2.new p.compiler
      end

      def bytecode(g)
      end
    end

    class Flip3 < Node
      def self.from(p, start, finish)
        Flip3.new p.compiler
      end

      def bytecode(g)
      end
    end

    class Float < Node
      def self.from(p, str)
        float = Literal.new p.compiler
        float.normalize str.to_f
      end
    end

    class File < Node
      def self.from(p)
        File.new p.compiler
      end

      def bytecode(g)
        pos(g)

        g.push_scope
        g.send :active_path, 0
      end
    end

    class For < Iter
      def self.from(p, iter, args, body)
        node = For.new p.compiler
        node
      end

      def bytecode(g)
      end
    end

    class GVar < Node
      def self.from(p, name)
        node = GVar.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        pos(g)

        if @name == :$!
          g.push_exception
        elsif @name == :$~
          g.push_variables
          g.send :last_match, 0
        else
          g.push_const :Rubinius
          g.find_const :Globals
          g.push_literal @name
          g.send :[], 1
        end
      end
    end

    class GVarAssign < Node
      def self.from(p, name, expr)
        node = GVarAssign.new p.compiler
        node.name = name
        node.value = expr
        node
      end

      def bytecode(g)
        pos(g)

        # @value can to be present if this is coming via an masgn, which means
        # the value is already on the stack.
        if @name == :$!
          @value.bytecode(g) if @value
          g.raise_exc
        elsif @name == :$~
          if @value
            g.find_cpath_top_const :Regexp
            @value.bytecode(g)
            g.send :last_match=, 1
          else
            g.find_cpath_top_const :Regexp
            g.swap
            g.send :last_match=, 1
          end
        else
          if @value
            g.push_const :Rubinius
            g.find_const :Globals
            g.push_literal @name
            @value.bytecode(g)
            g.send :[]=, 2
          else
            g.push_const :Rubinius
            g.find_const :Globals
            g.swap
            g.push_literal @name
            g.swap
            g.send :[]=, 2
          end
        end
      end
    end

    class HashLiteral < Node
      def self.from(p, array)
        node = HashLiteral.new p.compiler
        node.body = array
        node
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Hash

        @body.each { |x| x.bytecode(g) }

        g.send :[], @body.size
      end
    end

    class If < Node
      attr_accessor :condition, :body, :else

      def self.from(p, cond, body, else_body)
        node = If.new p.compiler
        node.condition = cond
        node.body = body || Nil.from(p)
        node.else = else_body || Nil.from(p)
        node
      end

      def children
        [@condition, @body, @else]
      end

      def bytecode(g)
        pos(g)

        done = g.new_label
        else_label = g.new_label

        @condition.bytecode(g)
        g.gif else_label

        @body.bytecode(g)
        g.goto done

        else_label.set!
        @else.bytecode(g)

        done.set!
      end
    end

    class Iter < Node
      attr_accessor :parent

      def self.from(p, iter, args, body)
        node = Iter.new p.compiler
        node
      end

      def variables
        @variables ||= {}
      end

      def allocate_slot
        variables.size
      end

      def nest_scope(scope)
        scepe.parent = self
      end

      # A nested scope is looking up a local variable. If the variable exists
      # in our local variables hash, return a nested reference to it. If it
      # exists in an enclosing scope, increment the depth of the reference
      # when it passes through this nested scope (i.e. the depth of a
      # reference is a function of the nested scopes it passes through from
      # the scope it is defined in to the scope it is used in).
      def search_local(name)
        if variable = variables[name]
          NestedLocalReference.new variable
        elsif reference = @parent.search_local(name)
          reference.depth += 1
          reference
        end
      end

      # If the local variable exists in this scope, set the local variable
      # node attribute to a reference to the local variable. If the variable
      # exists in an enclosing scope, set the local variable node attribute to
      # a nested reference to the local variable. Otherwise, create a local
      # variable in this scope and set the local variable node attribute.
      def assign_local_reference(var)
        if variable = variables[var.name]
          var.variable = LocalReference.new variable
        elsif reference = @parent.search_local(var.name)
          reference.depth += 1
          var.variable = reference
        else
          variable = LocalVar.new allocate_slot
          variables[var.name] = variable
          var.variable = LocalReference.new variable
        end
      end

      def bytecode(g)
      end
    end

    class IVar < Node
      def self.from(p, name)
        node = IVar.new p.compiler
        node.normalize name
        node
      end

      def bytecode(g)
        pos(g)

        g.push_ivar @name
      end
    end

    class IVarAssign < Node
      def self.from(p, name, expr)
        node = IVarAssign.new p.compiler
        node.normalize name, expr
        node
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g) if @value
        g.set_ivar @name
      end
    end

    class Literal < Node
      def self.from(p, sym)
        literal = Literal.new p.compiler
        literal.normalize sym
      end

      def bytecode(g)
        pos(g)

        g.push_unique_literal @value
      end
    end

    class LocalAccess < LocalVariable
      attr_accessor :variable

      def self.from(p, name)
        node = LocalAccess.new p.compiler
        node.name = name
        node
      end

      def bytecode(g)
        p @name unless @variable
        @variable.get_bytecode(g)
      end
    end

    class LocalAssignment < LocalVariable
      attr_accessor :depth

      def self.from(p, name, expr)
        node = LocalAssignment.new p.compiler
        node.name = name
        node.value = expr
        node
      end

      def children
        children = []
        children << @value if @value
        children
      end

      def bytecode(g)
        pos(g)

        if @value
          @value.bytecode(g)
        end

        @variable.set_bytecode(g)
      end
    end

    class MAsgn < Node
      attr_accessor :left, :right

      def self.from(p, left, right, flags)
        node = MAsgn.new p.compiler
        node.left = left
        node.right = right
        node
      end

      def bytecode(g)
      end
    end

    class Match < Node
      def self.from(p, pattern, flags)
        node = Match.new p.compiler
        node.pattern = RegexLiteral.from p, pattern, flags
        node
      end

      def bytecode(g)
        pos(g)

        g.push_const :Rubinius
        g.find_const :Globals
        g.push_literal :$_
        g.send :[], 1

        @pattern.bytecode(g)

        g.send :=~, 1
      end
    end

    class Match2 < Node
      attr_accessor :pattern, :value

      def self.from(p, pattern, value)
        node = Match2.new p.compiler
        node.pattern = pattern
        node.value = value
        node
      end

      def bytecode(g)
        pos(g)

        @pattern.bytecode(g)
        @value.bytecode(g)
        g.send :=~, 1
      end
    end

    class Match3 < Node
      attr_accessor :pattern, :value

      def self.from(p, pattern, value)
        node = Match3.new p.compiler
        node.pattern = pattern
        node.value = value
        node
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        @pattern.bytecode(g)
        g.send :=~, 1
      end
    end

    class MethodCall < Node
      def initialize(compiler)
        super compiler
        @scope = nil
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

      def children
        [@body]
      end

      def bytecode(g)
        pos(g)
        super(g)

        attach_and_call g, :__module_init__, true
      end
    end

    class Negate < Node
      attr_accessor :value

      def self.from(p, expr)
        node = Negate.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        pos(g)

        if @value.is? NumberLiteral
          g.push(-@value.value)
        else
          @value.bytecode(g)
          g.send :"-@", 0
        end
      end
    end

    class Next < Break
      def self.from(p, expr)
        node = Next.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        pos(g)

        g.pop_unwind if @pop_unwind

        if g.next
          g.goto g.next
        elsif @in_block
          if @value
            @value.bytecode(g)
          else
            g.push :nil
          end
          g.ret
        else
          @value.bytecode(g) if @value # next(raise("foo")) ha ha ha
          jump_error g, "next used in invalid context"
        end
      end
    end

    class Nil < Node
      def self.from(p)
        Nil.new p.compiler
      end

      def bytecode(g)
        g.push :nil
      end
    end

    class Not < Node
      attr_accessor :value

      def self.from(p, expr)
        node = Not.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        true_label = g.new_label
        end_label = g.new_label

        @value.bytecode(g)
        g.git true_label

        g.push :true
        g.goto end_label

        true_label.set!
        g.push :false
        end_label.set!
      end
    end

    class NthRef < Node
      def self.from(p, ref)
        node = NthRef.new p.compiler
        node.which = ref
        node
      end

      def bytecode(g)
        pos(g)

        g.push_variables
        g.push @which
        g.send :nth_ref, 1
      end
    end

    class NumberLiteral < Node
      def self.from(p, base, str)
        number = Literal.new p.compiler
        number.normalize(str.to_i(base))
      end

      def bytecode(g)
        g.push @value
      end
    end

    class OpAssign1 < Node
      attr_accessor :receiver, :op, :index, :value

      def self.from(p, receiver, index, op, value)
        node = OpAssign1.new p.compiler
        node.receiver = receiver
        node.op = op
        node.index = index
        node.value = value
        node
      end

      def children
        [@receiver, @value]
      end

      def bytecode(g)
      end
    end

    class OpAssign2 < Node
      def self.from(p, receiver, name, op, value)
        node = OpAssign2.new p.compiler
        node.object = receiver
        node.method = name
        node.kind = op
        node.value = value
        node.assign = name.to_s[-1] == ?= ? :"#{name}=" : name
        node
      end

      def children
        [@receiver, @value]
      end

      def bytecode(g)
      end
    end

    class OpAssignAnd < Node
      def self.from(p, left, right)
        node = OpAssignAnd.new p.compiler
        node.left = left
        node.right = right
        node
      end

      def children
        [@left, @right]
      end

      def bytecode(g)
        pos(g)

        @left.bytecode(g)
        lbl = g.new_label
        g.dup
        g.gif lbl
        g.pop
        @right.bytecode(g)
        lbl.set!
      end
    end

    class OpAssignOr < OpAssignAnd
      def self.from(p, left, right)
        node = OpAssignOr.new p.compiler
        node.left = left
        node.right = right
        node
      end

      def bytecode(g)
        pos(g)

        @left.bytecode_for_or(g) do
          @right.bytecode(g)
        end
      end
    end

    class Or < And
      def self.from(p, left, right)
        node = Or.new p.compiler
        node.left = left
        node.right = right
        node
      end

      def bytecode(g)
        super(g, false)
      end
    end

    class PushArgs < DynamicArguments
      def self.from(p, splat, value)
        node = PushArgs.new p.compiler
        node.array = splat.value
        node.item = value
        node
      end
    end

    class Range < Node
      def self.from(p, start, finish)
        node = Range.new p.compiler
        node.start = start
        node.finish = finish
        node
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.send :new, 2
      end
    end

    class RangeExclude < Range
      def self.from(p, start, finish)
        node = RangeExclude.new p.compiler
        node.start = start
        node.finish = finish
        node
      end

      def bytecode(g)
        pos(g)

        g.find_cpath_top_const :Range
        @start.bytecode(g)
        @finish.bytecode(g)
        g.push :true

        g.send :new, 3
      end
    end

    class Redo < Break
      def self.from(p)
        Redo.new p.compiler
      end

      def bytecode(g)
        g.pop_unwind if @pop_unwind

        if g.redo
          g.goto g.redo
        else
          jump_error g, "redo used in invalid context"
        end
      end
    end

    class RegexLiteral < Node
      def self.from(p, str, flags)
        node = RegexLiteral.new p.compiler
        node.source = str
        node.options = flags
        node
      end

      def bytecode(g)
        pos(g)

        # A regex literal should only be converted to a Regexp the first time it
        # is encountered. We push a literal nil here, and then overwrite the
        # literal value with the created Regexp if it is nil, i.e. the first time
        # only. Subsequent encounters will use the previously created Regexp
        idx = g.add_literal(nil)
        g.push_literal_at idx
        g.dup
        g.is_nil

        lbl = g.new_label
        g.gif lbl
        g.pop
        g.push_const :Regexp
        g.push_literal @source
        g.push @options
        g.send :new, 2
        g.set_literal idx
        lbl.set!
      end
    end

    class Rescue < Node
      def self.from(p, body, resc, els)
        node = Rescue.new p.compiler
        node.body = body
        node.rescue = resc
        node.else = els
        node
      end

      def children
        [@body, @rescue, @else]
      end

      def bytecode(g)
        pos(g)

        g.push_modifiers
        if @body.nil?
          if @else.nil?
            # Stupid. No body and no else.
            g.push :nil
          else
            # Only an else, run it.
            @else.bytecode(g)
          end
        else
          g.retry = g.new_label
          reraise = g.new_label
          els     = g.new_label
          done    = g.new_label

          # Save the current exception into a local
          g.push_exception

          g.retry.set!
          g.exceptions do |ex|
            @body.bytecode(g)
            ex.escape els

            ex.handle!
            @rescue.bytecode(g, reraise, done)
            reraise.set!
            g.reraise
          end

          els.set!
          if @else
            g.pop
            @else.bytecode(g)
          end
          done.set!

          g.swap
          g.pop_exception
        end
        g.pop_modifiers
      end
    end

    class RescueCondition < Node
      attr_accessor :conditions, :assignment, :body, :next, :splat

      def self.from(p, conditions, body, nxt)
        node = RescueCondition.new p.compiler
        node.next = nxt

        case conditions
        when ArrayLiteral
          node.conditions = conditions
        when ConcatArgs
          node.conditions = conditions.array
          node.splat = RescueSplat.from p, conditions.rest
        when SplatValue
          node.splat = RescueSplat.from p, conditions.value
        when nil
          condition = ConstFind.from p, :StandardError
          node.conditions = ArrayLiteral.from p, [condition]
        end

        case body
        when ArrayLiteral
          node.assignment = body.shift if assignment? body.first
          node.body = body
        when nil
          node.body = Nil.from p
        else
          if assignment? body
            node.assignment = body
            node.body = Nil.from p
          else
            node.body = body
          end
        end

        node
      end

      # TODO: simplify after assignment nodes are subclasses of Assignment
      def self.assignment?(node)
        (node.kind_of? LocalAssignment or
         node.kind_of? IVarAssign or
         node.kind_of? CVarAssign or
         node.kind_of? AttrAssign or
         node.kind_of? GVarAssign) and
        node.value.name == :$!
      end

      def map_rescue
        @body.in_rescue
        @body.visit do |result, node|
          case node
          when ClosedScope
            result = nil
          else
            node.in_rescue
          end

          result
        end
      end

      def children
        [@conditions, @assignment, @body, @next]
      end

      def bytecode(g, reraise, done)
        pos(g)
        body = g.new_label

        if @conditions
          @conditions.body.each do |c|
            c.bytecode(g)
            g.push_exception
            g.send :===, 1
            g.git body
          end
        end

        @splat.bytecode(g, body) if @splat

        if @next
          if_false = g.new_label
          g.goto if_false
        else
          g.goto reraise
        end

        body.set!

        if @assignment
          @assignment.bytecode(g)
          g.pop
        end

        current_break = g.break
        g.break = g.new_label

        map_rescue
        @body.bytecode(g)
        g.clear_exception
        g.goto done

        if g.break.used?
          g.break.set!
          g.clear_exception

          g.swap
          g.pop_exception
          if current_break
            g.goto current_break
          else
            g.raise_break
          end
        end

        g.break = current_break
        if @next
          if_false.set!
          @next.bytecode(g, reraise, done)
        end
      end
    end

    class RescueSplat < Node
      attr_accessor :value

      def self.from(p, value)
        node = RescueSplat.new p.compiler
        node.value = value
        node
      end

      def bytecode(g, body)
        @value.bytecode(g)
        g.cast_array
        g.push_exception
        g.send :__rescue_match__, 1
        g.git body
      end
    end

    class Retry < Break
      def self.from(p)
        Retry.new p.compiler
      end

      def bytecode(g)
        pos(g)

        if g.retry
          g.goto g.retry
        else
          jump_error g, "retry used in invalid context"
        end
      end
    end

    class Return < Node
      def self.from(p, expr)
        node = Return.new p.compiler
        node.value = expr
        node
      end

      def children
        [@value]
      end

      def in_rescue
        @in_rescue = true
      end

      def bytecode(g, force=false)
        pos(g)

        if @in_rescue
          g.clear_exception
        end

        # Literal ArrayList and a splat
        if @splat
          splat_node = @value.body.pop
          @value.bytecode(g)
          splat_node.call_bytecode(g)
          g.send :+, 1
        elsif @value
          @value.bytecode(g)
        else
          g.push :nil
        end

        if @in_block
          g.raise_return
        elsif !force and @in_ensure
          g.ensure_return
        else
          g.ret
        end
      end
    end

    class SClass < ClosedScope
      attr_accessor :receiver, :body

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
      def self.from(p, body)
        if body.kind_of? Block
          body
        elsif body
          Block.from p, [body]
        end
      end
    end

    class Self < Node
      def self.from(p)
        Self.new p.compiler
      end

      def bytecode(g)
        g.push :self
      end
    end

    class Snippit < ClosedScope
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

    class Splat < DynamicArguments
      attr_accessor :value

      def self.from(p, expr)
        node = Splat.new p.compiler
        node.value = expr
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        @value.bytecode(g)
        g.cast_array unless @value.kind_of? ArrayLiteral
      end
    end

    class StringLiteral < Node
      def self.from(p, str)
        node = StringLiteral.new p.compiler
        node.string = str
        node
      end

      def bytecode(g)
        pos(g)

        g.push_literal @string
        g.string_dup
      end
    end

    class Super < Call
      def self.from(p, args)
        node = Super.new p.compiler
        arguments = ArgList.new p.compiler
        arguments.body = args
        node.arguments = arguments
        node
      end

      def bytecode(g)
      end
    end

    class SValue < Node
      attr_accessor :value

      def self.from(p, expr)
        node = SValue.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        @value.bytecode(g)
        if @value.kind_of? SplatValue
          done = g.new_label

          g.dup
          g.send :size, 0
          g.push 1
          g.send :>, 1
          g.git done

          g.push 0
          g.send :at, 1

          done.set!
        end
      end
    end

    class ToArray < Node
      attr_accessor :value

      def self.from(p, expr)
        node = ToArray.new p.compiler
        node.value = expr
        node
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.cast_array
      end
    end

    class ToString < Node
      attr_accessor :value

      def self.from(p, value)
        node = ToString.new p.compiler
        node.value = value
        node
      end

      def children
        [@value]
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        g.send :to_s, 0, true
      end
    end

    class True < Node
      def self.from(p)
        True.new p.compiler
      end

      def bytecode(g)
        g.push :true
      end
    end

    class Undef < Node
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

    class Until < While
      def self.from(p, condition, body, check_first)
        node = Until.new p.compiler
        node.condition = condition
        node.body = body
        node.check_first = check_first
        node
      end

      def bytecode(g)
        super(g, false)
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

    class When < Node
      attr_accessor :conditions, :body

      def self.from(p, args, body)
        node = When.new p.compiler
        node.conditions = args
        node.body = body
        node
      end

      def bytecode(g, has_receiver, nxt, fin)
      end
    end

    class While < Node
      attr_accessor :condition, :body, :check_first

      def self.from(p, condition, body, check_first)
        node = While.new p.compiler
        node.condition = condition
        node.body = body
        node.check_first = check_first
        node
      end

      def bytecode(g, use_gif=true)
        pos(g)

        g.push_modifiers

        top = g.new_label
        bot = g.new_label
        g.break = g.new_label

        if @check_first
          g.redo = g.new_label
          g.next = top

          top.set!

          @condition.bytecode(g)
          if use_gif
            g.gif bot
          else
            g.git bot
          end

          g.redo.set!

          @body.bytecode(g)
          g.pop
        else
          g.next = g.new_label
          g.redo = top

          top.set!

          @body.bytecode(g)
          g.pop

          g.next.set!
          @condition.bytecode(g)
          if use_gif
            g.gif bot
          else
            g.git bot
          end
        end

        g.check_interrupts
        g.goto top

        bot.set!
        g.push :nil
        g.break.set!

        g.pop_modifiers
      end
    end

    class Yield < Call
      def self.from(p, expr, flags)
        node = Yield.new p.compiler
        node.arguments = expr
        node
      end

      def bytecode(g)
      end
    end

    class ZSuper < Super
      def self.from(p)
        ZSuper.new p.compiler
      end

      def bytecode(g)
      end
    end
  end
end
