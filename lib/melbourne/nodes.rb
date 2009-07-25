# Temporary location for modifications needed for any Compiler
# Node classes while getting Melbourne running.

# Use a different matcher while converting the parser/compiler
class CompileAsMatcher
  def matches?(actual)
    node = actual.to_ast
    generator = TestGenerator.new

    compiler = Compiler.new TestGenerator
    # TODO: Fix the compiler to have a proper interface for
    # enabling plugins. All compiler specs should be written
    # without plugins enabled, and each plugin should have
    # specs for bytecode with and without the plugin enabled.
    compiler.instance_variable_get(:@plugins).clear
    @plugins.each { |plugin| compiler.activate plugin }

    node.bytecode generator

    @actual = generator
    @actual == @expected
  end
end

class Compiler
  class Node
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
      attr_accessor :arg, :defs, :spl

      def self.from(p, args, defaults, splat)
        node = Arguments.new p.compiler
        node.arg = args
        node.defs = defaults
        node.spl = splat
      end
    end

    class ArrayLiteral < Node
      def self.from(p, array)
        node = ArrayLiteral.new p.compiler
        node.args array
        node
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

    class Block < Node
      def self.from(p, array)
        node = Block.new p.compiler
        node.body = array
        node
      end
    end

    class BlockAsArgument < Node
      def self.from(p, name)
        node = BlockAsArgument.new p.compiler
        node.args name
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

      def self.from(p, name, body)
        node = Define.new p.compiler
        node.body = body
        node
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
      def self.from(p, iter, args, body)
        node = For.new p.compiler
        node
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
      def self.from(p, name)
        node = LocalAccess.new p.compiler
        node.args name
        node
      end
    end

    class LocalAssignment < LocalVariable
      def self.from(p, name, expr)
        node = LocalAssignment.new p.compiler
        node.args name, expr
        node
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
        node
      end
    end

    class Self < Node
      def self.from(p)
        Self.new p.compiler
      end
    end

    class Splat < DynamicArguments
      def self.from(p, expr)
        node = Splat.new p.compiler
        node.args
        node
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
