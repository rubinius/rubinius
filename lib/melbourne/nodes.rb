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

    def visit(arg=true, &block)
      children.each do |child|
        next unless ch_arg = block.call(arg, child)
        child.visit(ch_arg, &block) if child
      end
    end

    class Alias < Node
      def self.from(p, to, from)
        node = Alias.new p.compiler
        node.args from, to
        node
      end
    end

    class And < Node
      def self.from(p, left, right)
        node = And.new p.compiler
        node.args left, right
        node
      end
    end

    class ArgList < ArrayLiteral
      def self.from(p, normal, splat)
        node = ArgList.new p.compiler
        normal.body << Splat.from(p, splat)
        node.args normal
        node
      end
    end

    class Arguments < Node
      attr_accessor :names, :required, :optional, :defaults, :splat
      attr_reader :block_arg

      def self.from(p, args, defaults, splat)
        node = Arguments.new p.compiler

        if defaults
          defaults = DefaultArguments.from p, defaults
          node.defaults = defaults
          node.optional = defaults.names

          stop = defaults.names.first
          last = args.each_with_index { |a, i| break i if a == stop }
          node.required = args[0, last]
        else
          node.required = args
          node.defaults = []
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
      end

      def children
        @defaults
      end
    end

    class DefaultArguments < Node
      attr_accessor :arguments, :names

      def self.from(p, block)
        node = DefaultArguments.new p.compiler
        node.arguments = block.body
        node.names = node.arguments.map { |a| a.name }
        node
      end

      def bytecode(g)
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
      def self.from(p, receiver, name, value)
        node = AttrAssign.new p.compiler
        node.args receiver, name, value
        node
      end
    end

    class BackRef < Node
      def self.from(p, ref)
        node = BackRef.new p.compiler
        node.args ref
        node
      end
    end

    class Begin < Node
      def self.from(p, body)
        node = Begin.new p.compiler
        node.args body
        node
      end
    end

    # Is it weird that Block has the :arguments attribute? Yes. Is it weird
    # that MRI parse tree puts arguments and block_arg in Block? Yes. So we
    # make do and pull them out here rather than having something else reach
    # inside of Block.
    class Block < Node
      def self.from(p, array)
        node = Block.new p.compiler
        node.body = array
        node
      end

      def strip_arguments
        if @body.first.kind_of? Arguments
          node = @body.shift
          if @body.first.kind_of? BlockArgument
            node.block_arg = @body.shift
          end
        end
        node
      end

      def children
        @body
      end
    end

    class BlockArgument < LocalVariable
      attr_accessor :name, :variable

      def self.from(p, name)
        node = BlockArgument.new p.compiler
        node.name = name
        node
      end
    end

    class BlockPass < Node
      def self.from(p, var, call)
        node = BlockPass.new p.compiler
      end
    end

    class Break < Node
      def self.from(p, expr)
        node = Break.new p.compiler
        node.args expr
        node
      end
    end

    class Call < MethodCall
      def self.from(p, receiver, sym, args)
        node = Call.new p.compiler
        node.object = receiver
        node.method = sym
        node.arguments = args
        node
      end
    end

    class Case < Node
      def self.from(p, expr, whens, els)
        node = Case.new p.compiler
        node.args expr, whens, els
        node
      end
    end

    class Class < ClosedScope
      def self.from(p, name, sup, body)
        node = Class.new p.compiler
        node.args name, nil, sup, body
        node
      end

      def bytecode(g)
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

      def find_local(name, in_block=false, allocate=true)
        raise "no find_local #{name.inspect}"
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
          case node
          when ClosedScope
            nil
          when LocalVariable
            scope.assign_local_reference node
            scope
          when Arguments
            scope.map_arguments node
            scope
          when Iter
            scope.nest_scope node
            scope = node
          else
            scope
          end
        end
      end

      def map_arguments(node)
        node.names.each { |arg| new_local arg }
      end

      def nest_scope(scope)
        scope.parent = self
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
      def self.from(p, outer, name)
        node = ConstAccess.new p.compiler
        node.args outer, name
        node
      end
    end

    class ConstAtTop < Node
      def self.from(p, name)
        node = ConstAtTop.new p.compiler
        node.args name
        node
      end
    end

    class ConstFind < Node
      def self.from(p, name)
        node = ConstFind.new p.compiler
        node.args name
        node
      end
    end

    class ConstSet < Node
      def self.from(p, expr, value)
        node = ConstSet.new p.compiler
        node.args expr, value
        node
      end
    end

    class CVar < Node
      def self.from(p, name)
        node = CVar.new p.compiler
        node.args name
        node
      end
    end

    class CVarAssign < Node
      def self.from(p, name, expr)
        node = CVarAssign.new p.compiler
        node.args name, expr
        node
      end
    end

    class CVarDeclare < CVarAssign
      def self.from(p, name, expr)
        node = CVarDeclare.new p.compiler
        node.args name, expr
        node
      end
    end

    class Define < ClosedScope
      attr_accessor :body

      def self.from(p, name, scope)
        node = Define.new p.compiler
        node.name = name
        node.arguments = scope.block.strip_arguments
        node.body = scope
        node
      end

      def compile_body(g)
        desc = new_description()
        meth = desc.generator

        prelude(g, meth)

        set(:scope, nil) do
          show_errors(meth) do
            @arguments.bytecode(meth)
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
        map_locals
        pos(g)

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
      def self.from(p, expr)
        node = Defined.new p.compiler
        node.args expr
        node
      end
    end

    class DefineSingleton < Define
      def self.from(p, receiver, name, body)
        node = DefineSingleton.new p.compiler
        node.object = receiver
        node.body = body
        node
      end
    end

    class DynamicExecuteString < DynamicString
      def self.from(p, str, array)
        node = DynamicExecuteString.new p.compiler
        node.string = str
        node.body = array
        node
      end
    end

    class DynamicRegex < DynamicString
      def self.from(p, str, array, flags)
        node = DynamicRegex.new p.compiler
        node.string = str
        node.body = array
        node.options = flags
        node
      end
    end

    class DynamicOnceRegex < DynamicRegex
      def self.from(p, str, array, flags)
        node = DynamicRegex.new p.compiler
        node.string = str
        node.body = array
        node.options = flags
        node
      end
    end

    class DynamicString < StringLiteral
      attr_accessor :options

      def self.from(p, str, array)
        node = DynamicString.new p.compiler
        node.string = str
        node.body = array
        node
      end
    end

    class DynamicSymbol < DynamicString
      def self.from(p, str, array)
        node = DynamicSymbol.new p.compiler
        node.string = str
        node.body = array
        node
      end
    end

    class EmptyArray < Node
      def self.from(p)
        EmptyArray.new p.compiler
      end
    end

    class Ensure < Node
      def self.from(p, head, ensr)
        node = Ensure.new p.compiler
        node
      end
    end

    class ExecuteString < StringLiteral
      def self.from(p, str)
        node = ExecuteString.new p.compiler
        node.args str
        node
      end
    end

    class False < Node
      def self.from(p)
        False.new p.compiler
      end
    end

    class Flip2 < Node
      def self.from(p, start, finish)
        Flip2.new p.compiler
      end
    end

    class Flip3 < Node
      def self.from(p, start, finish)
        Flip3.new p.compiler
      end
    end

    class Float < Node
      def self.from(p, str)
        float = Literal.new p.compiler
        float.normalize str.to_f
      end
    end

    class For < Iter
      def self.from(p, iter, args, body)
        node = For.new p.compiler
        node
      end
    end

    class GVar < Node
      def self.from(p, name)
        node = GVar.new p.compiler
        node.args name
        node
      end
    end

    class GVarAssign < Node
      def self.from(p, name, expr)
        node = GVarAssign.new p.compiler
        node.args name, expr
        node
      end
    end

    class HashLiteral < Node
      def self.from(p, array)
        node = HashLiteral.new p.compiler
        node.body = array
        node
      end
    end

    class If < Node
      def self.from(p, cond, body, else_body)
        node = If.new p.compiler
        node.args cond, body, else_body
        node
      end
    end

    class Iter < Node
      attr_accessor :parent

      def self.from(p, iter, args, body)
        node = For.new p.compiler
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
    end

    class IVarAssign < Node
      def self.from(p, name, expr)
        node = IVarAssign.new p.compiler
        node.normalize name, expr
        node
      end
    end

    class Literal < Node
      def self.from(p, sym)
        literal = Literal.new p.compiler
        literal.normalize sym
      end
    end

    class LocalAccess < LocalVariable
      attr_accessor :variable

      def self.from(p, name)
        node = LocalAccess.new p.compiler
        node.args name
        node
      end

      def bytecode(g)
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

      def bytecode(g)
        pos(g)

        if @value
          @value.bytecode(g)
        end

        @variable.set_bytecode(g)
      end
    end

    class MAsgn < Node
      def self.from(p, left, right, flags)
        node = MAsgn.new p.compiler
        node.args left, right
        node
      end
    end

    class Match < Node
      def self.from(p, pattern, flags)
        node = Match.new p.compiler
        node.args pattern
        node
      end
    end

    class Match2 < Node
      def self.from(p, pattern, value)
        node = Match2.new p.compiler
        node.args pattern, value
        node
      end
    end

    class Match3 < Node
      def self.from(p, pattern, value)
        node = Match3.new p.compiler
        node.args pattern, value
        node
      end
    end

    class MethodCall < Node
      def initialize(compiler)
        super compiler
        @scope = nil
      end
    end

    class Module < ClosedScope
      def self.from(p, name, body)
        node = Module.new p.compiler
        node.name = name
        node.body = body
        node
      end

      def bytecode(g)
      end
    end

    class Negate < Node
      def self.from(p, expr)
        node = Negate.new p.compiler
        node.args expr
        node
      end
    end

    class Next < Break
      def self.from(p, expr)
        node = Next.new p.compiler
        node.args expr
        node
      end
    end

    class Nil < Node
      def self.from(p)
        Nil.new p.compiler
      end
    end

    class Not < Node
      def self.from(p, expr)
        node = Not.new p.compiler
        node.args expr
        node
      end
    end

    class NthRef < Node
      def self.from(p, ref)
        node = NthRef.new p.compiler
        node.args ref
        node
      end
    end

    class NumberLiteral < Node
      def self.from(p, base, str)
        number = Literal.new p.compiler
        number.normalize(str.to_i(base))
      end
    end

    class OpAssign1 < Node
      def self.from(p, receiver, index, op, value)
        node = OpAssign1.new p.compiler
        node.args receiver, op, index, value
        node
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
    end

    class OpAssignAnd < Node
      def self.from(p, var, value)
        node = OpAssignAnd.new p.compiler
        node.args var, value
        node
      end
    end

    class OpAssignOr < OpAssignAnd
      def self.from(p, var, value)
        node = OpAssignOr.new p.compiler
        node.args var, value
        node
      end
    end

    class Or < And
      def self.from(p, left, right)
        node = Or.new p.compiler
        node.args left, right
        node
      end
    end

    class PushArgs < DynamicArguments
      def self.from(p, splat, value)
        node = PushArgs.new p.compiler
        node.array = splat.child
        node.item = value
        node
      end
    end

    class Range < Node
      def self.from(p, start, finish)
        node = Range.new p.compiler
        node.args start, finish
        node
      end
    end

    class RangeExclude < Range
      def self.from(p, start, finish)
        node = RangeExclude.new p.compiler
        node.args start, finish
        node
      end
    end

    class Redo < Break
      def self.from(p)
        Redo.new p.compiler
      end
    end

    class RegexLiteral < Node
      def self.from(p, str, flags)
        node = RegexLiteral.new p.compiler
        node.args str, flags
        node
      end
    end

    class Rescue < Node
      def self.from(p, body, resc, els)
        node = Rescue.new p.compiler
        node.args body, resc, els
        node
      end
    end

    class RescueCondition < Node
      def self.from(p, a, b, c)
        node = RescueCondition.new p.compiler
        node
      end
    end

    class Retry < Break
      def self.from(p)
        Retry.new p.compiler
      end
    end

    class Return < Node
      def self.from(p, expr)
        node = Return.new p.compiler
        node.args expr
        node
      end
    end

    class SClass < ClosedScope
      def self.from(p, receiver, body)
        node = SClass.new
        node.args receiver, body
        node
      end
    end

    class Scope < Node
      def self.from(p, body)
        node = Scope.new p.compiler
        if body.kind_of? Block
          node.block = body
        else
          node.block = Block.from p, body
        end
        node
      end

      def children
        [@block]
      end
    end

    class Self < Node
      def self.from(p)
        Self.new p.compiler
      end
    end

    class Snippit < ClosedScope
      def self.from(compiler, body)
        node = Snippit.new compiler
        node.args body
        node
      end

      def args(body)
        @body = expand body
      end

      def bytecode(g)
        map_locals
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

      def bytecode(g)
        g.make_array 0
        @value.bytecode(g)
        g.cast_array
        g.send :+, 1
      end
    end

    class StringLiteral < Node
      def self.from(p, str)
        node = StringLiteral.new p.compiler
        node.args str
        node
      end
    end

    class Super < Call
      def self.from(p, args)
        node = Super.new p.compiler
        arguments = ArgList.new p.compiler
        arguments.body = args
        node.arguments = arguments
        node.collapse_args
        node
      end
    end

    class SValue < Node
      def self.from(p, expr)
        node = SValue.new p.compiler
        node.args expr
        node
      end
    end

    class ToArray < Node
      def self.from(p, expr)
        node = ToArray.new p.compiler
        node.args expr
        node
      end
    end

    class ToString < Node
      def self.from(p, value)
        node = ToString.new p.compiler
        node.args value || ""
        node
      end
    end

    class True < Node
      def self.from(p)
        new p.compiler
      end
    end

    class Undef < Node
      def self.from(p, sym)
        literal = Literal.new(p.compiler).normalize(sym)
        node = Undef.new p.compiler
        node.name = literal.value
        node
      end
    end

    class Until < While
      def self.from(p, cond, body, post_cond)
        node = Until.new p.compiler
        node.args cond, body, post_cond
        node
      end
    end

    class VAlias < Node
      def self.from(p, to, from)
        node = VAlias.new p.compiler
        node.args from, to
        node
      end
    end

    class When < Node
      def self.from(p, args, body)
        node = When.new p.compiler
        node.conditions = args
        node.body = body
        node
      end
    end

    class While < Node
      def self.from(p, cond, body, post_cond)
        node = While.new p.compiler
        node.args cond, body, post_cond
        node
      end
    end

    class Yield < Call
      def self.from(p, expr, flags)
        node = Yield.new p.compiler
        node.args expr
        node
      end
    end

    class ZSuper < Super
      def self.from(p)
        ZSuper.new p.compiler
      end
    end
  end
end
