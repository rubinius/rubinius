# Implements methods on each Node subclass for generatng bytecode
# from itself.

require 'compiler/generator'

class Compiler
  class MethodDescription
    def initialize(gen_class, locals)
      @generator = gen_class.new
      @locals = locals
      @name = nil
      @required = 0
      @optional = 0
      @name = :__unknown__
    end

    attr_reader :generator, :locals
    attr_accessor :required, :optional, :name, :args

    def run(container, body)
      @generator.run(body)
      @required, @optional = container.argument_info
      @name = container.name
    end

    def ==(desc)
      desc.kind_of? MethodDescription and @generator == desc.generator
    end

    def to_cmethod
      @generator.to_cmethod(self)
    end

    def inspect
      [:method_description, @name, @required, @optional, @generator].inspect
    end
  end
end

class Compiler
class Node

  def show_errors(gen, &block)
    @compiler.show_errors(gen, &block)
  end

  class ClosedScope

    def new_description
      Compiler::MethodDescription.new(@compiler.generator_class, self.locals)
    end

    def to_description(name=nil)
      desc = new_description()
      desc.name = name if name
      gen = desc.generator

      show_errors(gen) do
        desc.run(self, self)
        gen.close
      end
      return desc
    end

    def argument_info
      [0, 0]
    end

    def attach_and_call(g, name)
      # If the body is empty, then don't bother with it.
      return if @body.empty?

      desc = new_description()
      meth = desc.generator

      prelude(g, meth)

      meth.push :self
      meth.set_encloser

      set(:scope, self) do
        show_errors(meth) do
          desc.run self, @body
        end
      end

      meth.sret
      meth.close

      g.dup
      g.push_literal desc
      g.swap
      g.attach_method name
      g.pop
      g.send name, 0
      g.push_encloser
    end

    def prelude(orig, g)

      if orig
        g.set_line orig.line, orig.file
      end

      # Allocate some stack to store locals.

      if @alloca > 0
        g.allocate_stack @alloca
      end
    end
  end

  class Snippit
    def bytecode(g)
      set(:scope, self) do
        prelude(nil, g)
        @body.bytecode(g)
      end
    end
  end

  class Expression
    def bytecode(g)
      set(:scope, self) do
        prelude(nil, g)
        @body.bytecode(g)
        g.sret
      end
    end
  end

  class EvalExpression
    def bytecode(g)
      ret = super(g)
      enlarge_context
      return ret
    end
  end

  class Script
    def bytecode(g)
      set(:scope, self) do
        prelude(nil, g)
        @body.bytecode(g)
        g.pop
        g.push :true
        g.sret
      end
    end
  end

  class Newline
    def bytecode(g)
      g.set_line @line, @file
      @child.bytecode(g) if @child
    end

    def call_bytecode(g)
      g.set_line @line, @file
      @child.call_bytecode(g)
    end
  end

  # TESTED
  class True
    def bytecode(g)
      g.push :true
    end
  end

  # TESTED
  class False
    def bytecode(g)
      g.push :false
    end
  end

  # TESTED
  class Nil
    def bytecode(g)
      g.push :nil
    end
  end

  # TESTED
  class Self
    def bytecode(g)
      g.push :self
    end
  end

  # TESTED
  class And
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

  # TESTED
  class Or
    def bytecode(g)
      super(g, false)
    end
  end

  # TESTED
  class Not
    def bytecode(g)
      @child.bytecode(g)
      tr = g.new_label
      ed = g.new_label
      g.git tr
      g.push :true
      g.goto ed
      tr.set!
      g.push :false
      ed.set!
    end
  end

  # TESTED
  class Negate
    def bytecode(g)
      if @child.is? NumberLiteral
        g.push -@child.value
      else
        @child.bytecode(g)
        g.send :"-@", 0
      end
    end
  end

  # TESTED
  class NumberLiteral
    def bytecode(g)
      g.push @value
    end
  end

  # TESTED
  class Literal
    def bytecode(g)
      g.push_unique_literal @value
    end
  end

  # TESTED
  class ArrayLiteral
    def bytecode(g)
      @body.each do |x|
        x.bytecode(g)
      end

      g.make_array @body.size
    end
  end

  # TESTED
  class EmptyArray
    def bytecode(g)
      g.make_array 0
    end
  end

  # TESTED
  class HashLiteral
    def bytecode(g)
      count = @body.size
      i = count - 1

      while i > 0
        v = @body[i]
        k = @body[i - 1]

        v.bytecode(g)
        k.bytecode(g)
        i -= 2
      end

      g.push_cpath_top
      g.find_const :Hash
      g.send :[], count
    end
  end

  # TESTED
  class StringLiteral
    def bytecode(g)
      g.push_literal @string
      g.string_dup
    end
  end

  # TESTED
  class ExecuteString
    def bytecode(g)
      super(g)
      g.push :self
      g.send :`, 1, true # ` (silly vim/emacs)
    end
  end

  # TESTED
  class ToString
    def bytecode(g)
      @child.bytecode(g)
      g.send :to_s, 0, true
    end
  end

  # TESTED
  class DynamicString
    def bytecode(g)
      @body.reverse_each do |x|
        x.bytecode(g)
      end
      g.push_literal @string
      g.string_dup

      @body.size.times do
        g.string_append
      end
    end
  end

  # TESTED
  class DynamicExecuteString
    def bytecode(g)
      super(g)
      g.push :self
      g.send :`, 1, true #`
    end
  end

  # TESTED
  class DynamicSymbol
    def bytecode(g)
      @string.bytecode(g)
      g.send :to_sym, 0, true
    end
  end

  # TESTED
  class RegexLiteral
    def bytecode(g)
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
      g.push @options
      g.push_literal @source
      g.push_const :Regexp
      g.send :new, 2
      g.set_literal idx
      lbl.set!
    end
  end

  # TESTED
  class DynamicRegex
    def bytecode(g)
      g.push @options

      super(g)

      g.push_const :Regexp
      g.send :new, 2
    end
  end

  # TESTED
  class DynamicOnceRegex
    def bytecode(g)
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

  # TESTED
  class Match2
    def bytecode(g)
      @target.bytecode(g)
      @pattern.bytecode(g)
      g.send :=~, 1
    end
  end

  # TESTED
  class Match3
    def bytecode(g)
      @pattern.bytecode(g)
      @target.bytecode(g)
      g.send :=~, 1
    end
  end

  # TESTED
  class BackRef
    def bytecode(g)
      g.push_literal @kind
      g.push_context
      g.send :back_ref, 1
    end
  end

  # TESTED
  class NthRef
    def bytecode(g)
      g.push @which
      g.push_context
      g.send :nth_ref, 1
    end
  end

  # TESTED
  class Block
    def bytecode(g)
      if @body.empty?
        g.push :nil
        return
      end

      count = @body.size - 1
      i = 0
      while i < count
        ip = g.ip
        @body[i].bytecode(g)

        # guards for things that plugins might optimize away.
        g.pop if g.advanced_since?(ip)
        i += 1
      end
      @body[count].bytecode(g)
    end
  end

  # TESTED
  class Scope
    def bytecode(g)
      if @block.nil?
        g.push :nil
        return
      end

      @block.bytecode(g)
    end
  end

  # TESTED
  class If
    def bytecode(g)
      ed = g.new_label
      el = g.new_label

      @condition.bytecode(g)

      if @then and @else
        g.gif el
        @then.bytecode(g)
        g.goto ed
        el.set!
        @else.bytecode(g)
      elsif @then
        g.gif el
        @then.bytecode(g)
        g.goto ed
        el.set!
        g.push :nil
      elsif @else
        g.git el
        @else.bytecode(g)
        g.goto ed
        el.set!
        g.push :nil
      else
        # An if with no code. Sweet.
        g.pop
        g.push :nil
        return
      end

      ed.set!
    end
  end

  # TESTED
  class While
    def bytecode(g, use_gif=true)
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

      g.goto top

      bot.set!
      g.push :nil
      g.break.set!

      g.pop_modifiers
    end
  end

  # TESTED
  class Until
    def bytecode(g)
      super(g, false)
    end
  end

  # TESTED
  class Loop
    def bytecode(g)
      g.push_modifiers

      g.break = g.new_label
      g.next = g.redo = top = g.new_label
      top.set!

      @body.bytecode(g)
      g.pop

      g.goto top

      g.break.set!
      g.pop_modifiers
    end
  end

  # TESTED
  class Iter
    def argument_info
      [@arguments.arity, @arguments.optional.size]
    end

    def bytecode(g)
      desc = Compiler::MethodDescription.new @compiler.generator_class, @locals
      desc.name = :__block__
      desc.required, desc.optional = argument_info
      sub = desc.generator

      # Push line info down.
      sub.set_line g.line, g.file

      show_errors(sub) do
        if @arguments
          @arguments.bytecode(sub)
        else
          # Remove the block args.
          sub.pop
        end
        sub.push_modifiers
        sub.break = nil
        sub.next = nil
        sub.redo = sub.new_label
        sub.redo.set!
        @body.bytecode(sub)
        sub.pop_modifiers
        sub.soft_return
        sub.close
      end

      g.push_literal desc
      g.create_block2
    end
  end

  # TESTED
  class BlockPass
    def bytecode(g)
      @block.bytecode(g)
    end
  end

  # TESTED
  class Break

    def do_value(g)
      if @value
        @value.bytecode(g)
      else
        g.push :nil
      end
    end

    def jump_error(g, msg)
      g.push_literal msg
      g.push_const :LocalJumpError
      g.push :self
      g.send :raise, 2, true
    end

    def bytecode(g)
      do_value(g)

      if g.break
        g.goto g.break
      elsif @in_block
        # Set the return value from @value above.
        g.push_local @check_var.slot
        g.send :break_value=, 1
        g.pop

        # Now raise it.
        g.push_local @check_var.slot
        g.raise_exc
      else
        g.pop
        g.push_const :Compile
        g.send :__unexpected_break__, 0
      end
    end
  end

  # TESTED
  class Redo
    def bytecode(g)
      if g.redo
        g.goto g.redo
      else
        jump_error g, "redo used in invalid context"
      end
    end
  end

  class Next
    def bytecode(g)
      if g.next
        g.goto g.next
      elsif @in_block
        if @value
          @value.bytecode(g)
        else
          g.push :nil
        end
        g.soft_return
      else
        jump_error g, "next used in invalid context"
      end
    end
  end

  class Retry
    def bytecode(g)
      if g.retry
        g.goto g.retry
      else
        jump_error g, "retry used in invalid context"
      end
    end
  end

  # TESTED
  class When
    # The bytecode to test a when condition is different, depending on whether
    # or not the case statement has a receiver, i.e.
    #   case foo; when ...
    # vs
    #   case; when ...
    def condition_bytecode(g, has_receiver, cond)
      if has_receiver
        # Case
        g.dup
        cond.bytecode(g)
        g.send :===, 1
      else
        # ManyIf - no receiver
        cond.bytecode(g)
      end
    end

    def bytecode(g, has_receiver, nxt, fin)
      if @conditions.size == 1 and !@splat
        condition_bytecode(g, has_receiver, @conditions.first)
        g.gif nxt
      else
        body = g.new_label

        @conditions.each do |c|
          condition_bytecode(g, has_receiver, c)
          g.git body
        end

        if @splat
          if @splat.kind_of? Array
            @splat.each do |c|
              condition_bytecode(g, has_receiver, c)
              g.git body
            end
          elsif has_receiver
            g.dup
            @splat.bytecode(g)
            g.cast_array
            g.send :__matches_when__, 1
            g.git body
          else
            raise Error, "encountered splat on many-if!"
          end
        end

        g.goto nxt

        body.set!
      end

      # Remove the thing we've been testing.
      g.pop if has_receiver
      @body.bytecode(g)
      g.goto fin
    end
  end

  # TESTED
  class Case
    def bytecode(g)
      fin = g.new_label

      @receiver.bytecode(g) if has_receiver?
      @whens.each do |w|
        nxt = g.new_label
        w.bytecode(g, has_receiver?, nxt, fin)
        nxt.set!
      end

      # The condition is still on the stack
      g.pop if has_receiver?
      if @else
        @else.bytecode(g)
      else
        g.push :nil
      end

      fin.set!
    end
  end

  # TESTED
  class LocalAssignment
    def bytecode(g)
      if @value
        @value.bytecode(g)
      end

      # No @value means assume that someone else put the value on the
      # stack (ie, an masgn)

      if @variable.on_stack?
        if @variable.argument?
          raise Error, "Invalid access semantics for argument: #{@name}"
        end

        g.set_local_fp @variable.stack_position
      elsif @variable.created_in_block?
        g.set_local_depth @depth, @variable.slot
      else
        g.set_local @variable.slot
      end
    end
  end

  # TESTED
  class LocalAccess
    def bytecode(g)
      if @variable.on_stack?
        if @variable.argument?
          g.from_fp @variable.stack_position
        else
          g.get_local_fp @variable.stack_position
        end
      elsif @variable.created_in_block?
        g.push_local_depth @depth, @variable.slot
      else
        g.push_local @variable.slot
      end
    end
  end

  class BlockAsArgument
    def bytecode(g)
      g.set_local @variable.slot
    end
  end

  # TESTED
  class SValue
    def bytecode(g)
      @child.bytecode(g)
      g.cast_array

      # If the array has 1 or 0 elements, grab the 0th element.
      # otherwise, leave the array on the stack.

      g.dup
      g.send :size, 0
      g.push 1
      g.send :<, 1

      lbl = g.new_label
      g.git lbl

      g.push 0
      g.swap
      g.send :at, 1

      lbl.set!
    end
  end

  # TESTED
  class Splat
    def bytecode(g)
      @child.bytecode(g)
      g.cast_array
    end

    # Bytecode generation when a splat is used as a method arg
    def call_bytecode(g)
      @child.bytecode(g)
      g.cast_array_for_args 0
      g.push_array
    end
  end

  # TESTED
  class OpAssignOr
    def bytecode(g, use_gif=false)
      @left.bytecode(g)
      lbl = g.new_label
      g.dup
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

  # TESTED
  class OpAssignAnd
    def bytecode(g)
      super(g, true)
    end
  end

  # TESTED
  class OpAssign1
    def index_bytecode(g)
      @index.reverse.each do |idx|
        idx.bytecode(g)
        g.swap
      end
    end

    def bytecode(g)
      fnd = g.new_label
      fin = g.new_label

      @object.bytecode(g)
      g.dup
      index_bytecode(g)
      g.send :[], @index.size

      if @kind == :or or @kind == :and
        g.dup
        if @kind == :or
          g.git fnd
        else
          g.gif fnd
        end
        g.pop
        @value.bytecode(g)
      else
        @value.bytecode(g)
        g.swap
        g.send @kind, 1
        g.swap
        index_bytecode(g)
        g.send :[]=, @index.size + 1
        return
      end

      g.swap
      index_bytecode(g)
      g.send :[]=, @index.size + 1
      g.goto fin

      fnd.set!
      g.swap
      g.pop

      fin.set!
    end
  end

  # TESTED
  class OpAssign2
    def bytecode(g)
      fnd = g.new_label
      fin = g.new_label
      @object.bytecode(g)
      g.dup
      g.send @method, 0

      if @kind == :or or @kind == :and
        g.dup
        if @kind == :or
          g.git fnd
        else
          g.gif fnd
        end
        g.pop
        @value.bytecode(g)
      else
        @value.bytecode(g)
        g.swap
        g.send @kind, 1
        g.swap
        g.send @assign, 1
        return
      end

      g.swap
      g.send @assign, 1
      g.goto fin

      fnd.set!
      g.swap
      g.pop

      fin.set!
    end
  end

  # TESTED
  class ConcatArgs
    def call_bytecode(g)
      @array.bytecode(g)
      g.cast_array_for_args @rest.size
      g.push_array
      g.get_args
      @rest.reverse_each do |x|
        x.bytecode(g)
        g.swap
      end
    end

    def bytecode(g)
      @array.bytecode(g)
      g.cast_array
      @rest.each do |x|
        x.bytecode(g)
      end
      g.make_array @rest.size
      g.send :+, 1
    end

  end

  # TESTED
  class PushArgs
    def bytecode(g)
      @item.bytecode(g)
      @array.bytecode(g)
      g.cast_array_for_args 1
      g.push_array
    end

    # Used for h[*x] = 3
    def attr_bytecode(g)
      @item.bytecode(g)
      g.dup
      @array.bytecode(g)
      g.cast_array_for_args 1
      g.push_array
    end
  end

  # TESTED
  class AccessSlot
    def bytecode(g)
      g.push_my_field @index
    end
  end

  # TESTED
  class SetSlot
    def bytecode(g)
      @value.bytecode(g)
      g.store_my_field @index
    end
  end

  # TESTED
  class IVar
    def bytecode(g)
      g.push_ivar @name
    end
  end

  # TESTED
  class IVarAssign
    def bytecode(g)
      @value.bytecode(g) if @value
      g.set_ivar @name
    end
  end

  # TESTED
  class GVar
    def bytecode(g)
      if @name == :$!
        g.push_exception
      elsif @name == :$~
        g.push_cpath_top
        g.find_const :Regexp
        g.send :last_match, 0
      else
        g.push_literal @name
        g.push_cpath_top
        g.find_const :Globals
        g.send :[], 1
      end
    end
  end

  # TESTED
  class GVarAssign
    def bytecode(g)
      @value.bytecode(g) if @value

      if @name == :$!
        g.raise_exc
        return
      elsif @name == :$~
        g.push_cpath_top
        g.find_const :Regexp
        g.send :my_last_match=, 1
        return
      end

      g.push_literal @name
      g.push_cpath_top
      g.find_const :Globals
      g.send :[]=, 2
    end
  end

  # TESTED
  class ConstFind
    def bytecode(g)
      g.push_const @name
    end
  end

  # TESTED
  class ConstAccess
    def bytecode(g)
      @parent.bytecode(g)
      g.find_const @name
    end
  end

  # TESTED
  class ConstAtTop
    def bytecode(g)
      g.push_cpath_top
      g.find_const @name
    end
  end

  # TESTED
  class ConstSet
    def bytecode(g)
      if @compiler.kernel?
        if @parent
          @parent.bytecode(g)
          @value.bytecode(g)
          g.set_const @name, true
        elsif @from_top
          g.push_cpath_top
          @value.bytecode(g)
          g.set_const @name, true
        else
          @value.bytecode(g) if @value
          g.set_const @name
        end
      else
        @value.bytecode(g) if @value
        g.push_literal @name

        if @parent
          @parent.bytecode(g)
        elsif @from_top
          g.push_cpath_top
        else
          g.push :self
        end

        g.send :__const_set__, 2
      end # @compiler.kernel?
    end
  end

  # TESTED
  class ToArray
    def bytecode(g)
      @child.bytecode(g)
      g.cast_array
    end
  end

  # TESTED
  class SClass
    def bytecode(g)
     @object.bytecode(g)
     g.dup
     g.send :__verify_metaclass__, 0
     g.pop
     g.open_metaclass

     attach_and_call g, :__metaclass_init__
    end
  end

  # TESTED
  class Class

    def superclass_bytecode(g)
      if @superclass
        @superclass.bytecode(g)
      else
        g.push :nil
      end
    end

    def bytecode(g)
      if @parent
        @parent.bytecode(g)
        superclass_bytecode(g)
        g.open_class_under @name
      else
        superclass_bytecode(g)
        g.open_class @name
      end

      attach_and_call g, :__class_init__
    end
  end

  # TESTED
  class Module

    def bytecode(g)
      if @parent
        @parent.bytecode(g)
        g.open_module_under @name
      else
        g.open_module @name
      end

      attach_and_call g, :__module_init__
    end
  end

  class Defined

    # e.g. [[:const, :Object], :Blah]
    # e.g. [[:colon3, :Foo], :Bar]
    # e.g. [[:colon2, [:colon3, :Faz], :Boo], :Batch]
    # e.g. [[:colon2, [:const, :Zizz], :Koom], :Yonk]
    # TODO - There is probably a better way, but it is late. Really late.
    def const_to_string(tree, str)
      return str if tree.empty?
      piece = tree.shift
      unless str[-2,2] == "::" || str == ""
        str << "::"
      end
      if piece.is_a?(Array)
        str = const_to_string(piece, str) until piece.empty?
        str
      elsif [:const, :colon2].include?(piece)
        str
      elsif piece == :colon3
        "::" + str
      else
        str << "#{piece}"
      end
    end

    def reject(g)
      if $DEBUG
        STDERR << "Passed a complex expression to 'defined?'"
      end

      g.push :false
    end

    def bytecode(g)
      # Imported directly from compiler1 and reworked to use g.

      expr = @expression.dup

      # if something is defined, !something is too.
      # if !something is undefined, then so is something.
      expr.shift if expr[0] == :not

       # grouped expression == evil
      if expr.flatten.include?(:newline)
        reject(g)
        return
      end

      node = expr.shift

      static_nodes = [:self, :nil, :true, :false, :lit, :lasgn, :gasgn, :iasgn, :cdecl, :cvdecl, :cvasgn, :lvar, :str, :array, :hash]
      if static_nodes.include?(node)
        g.push :true
        return
      end

      case node
      when :call
        receiver = expr.shift
        msg = expr.shift # method name

        # Make sure there are no args.
        unless expr.empty?
          reject(g)
          return
        end

        g.push_literal msg
        receiver.bytecode(g)
        g.send :respond_to?, 1
      when :cvar
        cvar = expr.shift
        g.push_literal cvar
        # class vars as symbols, not strings
        g.push :true
        g.push :self
        g.send :class_variables, 1
        g.send :include?, 1
      when :gvar
        g.push_literal expr.shift
        g.push_const :Globals
        g.send :key?, 1
      when :ivar
        ivar = expr.shift
        g.push_literal ivar
        g.push :self
        g.send :instance_variable_defined?, 1
      when :yield
        g.push_block
      when :const
        g.push_literal expr.shift
        g.push_context
        g.send :const_defined?, 1
      when :colon2
        str = ""
        until expr.empty?
          # Convert the constant parse tree into a string like ::Object::SomeClass
          str = const_to_string(expr, str)
        end
        g.push_literal str
        g.push_context
        g.send :const_defined?, 1
      when :colon3
        str = ""
        until expr.empty?
          # Convert the constant parse tree into a string like ::Object::SomeClass
          str = const_to_string(expr, str)
        end
        g.push_literal str
        g.push_const :Object
        g.send :const_defined?, 1
      else
        reject(g)
      end
    end
  end

  class Begin
    def bytecode(g)
      @body.bytecode(g)
    end
  end

  # TESTED
  class RescueCondition
    def bytecode(g, top_if_false, if_done)
      body = g.new_label

      if @conditions
        @conditions.each do |x|
          g.push_exception
          x.bytecode(g)
          g.send :===, 1
          g.git body
        end
      end

      if @splat
        g.push_exception
        @splat.bytecode(g)
        g.cast_array
        g.send :__rescue_match__, 1
        g.git body
      end

      if @next
        # There are rescues
        if_false = g.new_label

        g.goto if_false
        body.set!
        @body.bytecode(g)
        g.goto if_done

        if_false.set!
        @next.bytecode(g, top_if_false, if_done)
      else
        # This is the last rescue
        g.goto top_if_false

        body.set!
        @body.bytecode(g)
        g.clear_exception
        g.goto if_done
      end
    end
  end

  # TESTED
  class Rescue
    def bytecode(g)
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
        rr  = g.new_label
        fls = g.new_label
        els = g.new_label
        last = g.new_label

        g.exceptions do |ex|
          g.retry.set!
          @body.bytecode(g)
          g.goto els

          ex.handle!
          @rescue.bytecode(g, rr, last)
          rr.set!
          g.push_exception
          g.raise_exc
        end

        els.set!
        if @else
          g.pop
          @else.bytecode(g)
        end
        last.set!
      end
      g.pop_modifiers
    end
  end

  class Ensure
    def bytecode(g)
      if @body.nil?
        # So dumb. Oh well.

        if @ensure
          # Well, run it.
          @ensure.bytecode(g)
          g.pop
        end

        g.push :nil

        return
      end

      # No ensure? dumb.
      if @ensure.nil?
        @body.bytecode(g)
        return
      end

      ok = g.new_label
      g.exceptions do |ex|
        @body.bytecode(g)
        g.goto ok

        ex.handle!

        @ensure.bytecode(g)
        g.pop
        # Re-raise the exception
        g.push_exception
        if @did_return and !@outer_ensure
          g.dup
          g.push_const :ReturnException
          g.send :===, 1

          after = g.new_label
          g.gif after
          g.send :return_value, 0
          g.clear_exception
          g.ret

          after.set!
        end

        g.raise_exc
      end

      ok.set!

      # Now, re-emit the code for the ensure which will run if there was no
      # exception generated.
      @ensure.bytecode(g)
      g.pop
    end
  end

  # TESTED
  class Return
    def bytecode(g, force=false)
      if @in_rescue
        g.clear_exception
      end

      if @value
        @value.bytecode(g)
      else
        g.push :nil
      end

      if !force and @in_ensure
        g.push_cpath_top
        g.find_const :ReturnException
        g.send :new, 1
        g.raise_exc
        return
      end

      if @in_block
        # Set the return value from @value above.
        g.push_local @check_var.slot
        g.send :return_value=, 1
        g.pop

        # Now raise it.
        g.push_local @check_var.slot
        g.raise_exc
      else
        g.sret
      end
    end
  end

  # TESTED
  class IterArgs
    def bytecode(g)
      case @child
      when MAsgn
        g.cast_for_multi_block_arg unless @child.splat_only?
        @child.in_block = true
        @child.bytecode(g)
      when LocalAssignment, IVarAssign, GVarAssign
        g.cast_for_single_block_arg
        @child.bytecode(g)
        g.pop
      when AttrAssign
        g.cast_for_single_block_arg
        @child.bytecode(g, true)
        g.pop
      when Fixnum, nil
        g.pop
      else
        raise Error, "Unknown form of block args: #{@child.class}"
      end
    end
  end

  # TESTED
  class MAsgn
    # MAsgn rules:
    # - MAsgn sexps always contain either 2 or 3 values
    # - If the masgn contains only 2 values:
    #   - it means there are no lasgns, i.e. the lhs consists solely of a splat
    #     arg.
    #   - the first arg is the lhs splat arg, and cannot be nil; however, it
    #     may be just the value true, in the case of a block anonymous splat arg,
    #     i.e. |*|
    #   - the second arg is either one of [:array, :splat, :argscat, :to_ary], or
    #     nil if the masgn represents block arguments.
    # - If the masgn contains 3 values:
    #   - the first arg is an array of lasgns with a minimum of one lasgn.
    #   - the second arg is the lhs splat arg, or nil if there are no splats
    #   - the third arg is either one of [:array, :splat, :argscat, :to_ary], or
    #     nil if the masgn represents block arguments.
    # - An MAsgn may contain nested masgns, e.g. a,(b,c) = 1,[2,3]. The nested
    #   masgn(s) will have nil for their @assigns, since the assigns come from
    #   the outermost masgn.
    def bytecode(g)
      if @source
        if @source.is? ArrayLiteral
          if @splat
            array_bytecode(g)
          else
            flip_assign_bytecode(g)
          end
        else
          statement_bytecode(g)
        end
      elsif @in_block
        block_arg_bytecode(g)
      else
        statement_bytecode(g)
      end
    end

    # Pad stack if there are insufficient source values for the assigns
    def pad_stack(g)
      diff = -@source.body.size
      if @assigns
        diff += @assigns.body.size
        if diff > 0
          diff.times { g.push :nil }
        end
      end
      return diff
    end

    # The easiest case for a normal masgn.
    # a,b = c,d
    # a = c,d
    # a,b,c = d,e
    def flip_assign_bytecode(g)
      # Pad the stack with extra nils if there are more assigns
      # than sources
      diff = pad_stack(g)

      @source.body.reverse_each do |x|
        x.bytecode(g)
      end

      # Now all the source data is on the stack.

      @assigns.body.each do |x|
        if x.is? AttrAssign
          x.bytecode(g, true)
        else
          x.bytecode(g)
        end
        g.pop
      end

      # Clean up the stack if there was extra sources
      if diff < 0
        # unary - has binds strangly
        (-diff).times { g.pop }
      end

      g.push :true
    end

    def array_bytecode(g)
      @source.body.each do |x|
        x.bytecode(g)
      end

      diff = pad_stack(g)

      if diff >= 0
        sz = 0
      else
        sz = -diff
      end

      g.make_array sz
      @splat.bytecode(g) unless @splat.kind_of? TrueClass  # e.g. * = 1,2
      g.pop
      if @assigns
        @assigns.body.reverse_each do |x|
          if x.is? AttrAssign
            x.bytecode(g, true)
          else
            x.bytecode(g)
          end
          g.pop
        end
      end

      g.push :true
    end

    def statement_bytecode(g)
      if @source.nil?
        # skip
      elsif @source.is? Splat or @source.is? ToArray
        @source.child.bytecode(g)
      elsif @source.is? ConcatArgs
        @source.bytecode(g)
      elsif @source
        raise Error, "Unknown form: #{@source.class}"
      end
      g.cast_tuple

      if @assigns
        @assigns.body.each do |x|
          g.unshift_tuple
          if x.is? AttrAssign
            x.bytecode(g, true)
          else
            x.bytecode(g)
          end
          g.pop
        end
      end

      if @splat
        g.cast_array
        @splat.bytecode(g) unless @splat.kind_of? TrueClass # HACK dup
      end
      g.pop

      g.push :true
    end

    def block_arg_bytecode(g)
      if @assigns
        @assigns.body.each do |x|
          g.unshift_tuple
          x.bytecode(g)
          g.pop
        end
      end

      if @splat and !@splat.kind_of? TrueClass
        g.cast_array
        @splat.bytecode(g)
      end

      g.pop
    end

    def splat_only?
      @assigns.nil? and @splat
    end
  end

  # TESTED
  class FCall
    def allow_private?
      true
    end

    def receiver_bytecode(g)
      g.push :self
    end

  end

  # TESTED
  class AttrAssign
    def bytecode(g, in_masgn = false)
      if in_masgn
        @in_masgn = true
        super(g)
      else
        super(g)
        g.pop
        # The dup'd rhs is on the top of the stack now
      end
    end

    def emit_args(g)
      if @in_masgn
        if @rhs_expression
          extra = 2
          @rhs_expression.bytecode(g)
        else
          extra = 1
        end

        super(g)

        @argcount += extra
      elsif @rhs_expression
        @rhs_expression.bytecode(g)
        g.dup
        super(g)
        @argcount += 1
      else
        # PushArgs only for this branch
        @arguments.attr_bytecode(g)
        g.get_args
        @dynamic = true
      end
    end
  end

  # TESTED
  class Call
    def allow_private?
      false
    end

    def receiver_bytecode(g)
      @object.bytecode(g)
    end

    def emit_args(g)
      @dynamic = false
      if @arguments
        if @arguments.kind_of? Array
          @arguments.reverse_each do |x|
            x.bytecode(g)
          end
          @argcount = @arguments.size
        elsif @arguments.is? ConcatArgs
          @arguments.call_bytecode(g)
          # ConcatArgs calls get_args on its own, so we don't need to
          @dynamic = true
        else
          if @arguments.is? Splat
            @arguments.call_bytecode(g)
          else
            @arguments.bytecode(g)
          end
          g.get_args
          @dynamic = true
        end
      else
        @argcount = 0
      end
    end

    def bytecode(g)
      return if use_plugin(g, :call)

      emit_args(g)

      if @block
        @block.bytecode(g)
      elsif @dynamic
        g.push :nil
      end

      g.swap if @dynamic

      receiver_bytecode(g)

      # @block might be BlockPass, and we don't generate the LRE detection
      # code for that.
      if @block and @block.is? Iter
        block_bytecode(g)
      elsif @dynamic
        g.swap
        g.set_args
        g.send_with_register @method, allow_private?
      elsif @block
        # Only BlockPass currently
        g.send_with_block @method, @argcount, allow_private?
      else
        g.send @method, @argcount, allow_private?
      end
    end

    def block_bytecode(g)
      ok = g.new_label
      g.exceptions do |ex|

        # We pre-create the exception.
        g.push_cpath_top
        g.find_const :LongReturnException
        g.send :allocate, 0
        g.set_local @check_var.slot
        g.pop

        if @dynamic
          g.swap
          g.set_args
          g.send_with_register @method, allow_private?
        else
          g.send_with_block @method, @argcount, allow_private?
        end

        g.goto ok

        ex.handle!

        g.push_exception
        g.dup
        g.push_local @check_var.slot
        g.equal

        after = g.new_label
        g.gif after
        g.clear_exception

        # This is also used for break in a block. If break was used,
        # is_return is false, so we just leave the value on the stack.
        leave = g.new_label
        g.dup
        g.send :is_return, 0
        g.gif leave

        g.send :value, 0
        g.ret

        after.set!

        g.raise_exc

        leave.set!
        g.send :value, 0
      end

      ok.set!
    end
  end

  # TESTED
  class Yield
    def bytecode(g)
      emit_args(g)

      if @dynamic
        g.set_args
        g.push :nil
        g.push_block
        g.send_with_register :call, false
      else
        g.push_block
        g.meta_send_call @argcount
      end

    end
  end

  class Super
    def bytecode(g)
      emit_args(g)

      if @block
        @block.bytecode(g)
      else
        g.push :nil
      end

      if @dynamic
        g.swap
        g.set_args
        g.send_super @method.name
      else
        g.send_super @method.name, @argcount
      end
    end
  end

  class ZSuper

    def bytecode(g)
      args = []

      @method.arguments.required.each do |var|
        la = LocalAccess.new @compiler
        la.from_variable var
        args << la
      end

      @method.arguments.optional.each do |var|
        la = LocalAccess.new @compiler
        la.from_variable var
        args << la
      end

      if @method.arguments.splat
        cc = ConcatArgs.new @compiler
        la = LocalAccess.new @compiler
        la.from_variable @method.arguments.splat

        cc.args args, la
        @arguments = cc
      else
        @arguments = args
      end

      super(g)
    end
  end

  # TESTED
  class Undef
    def bytecode(g)
      g.push_literal @name
      g.push :self
      unless @in_module
        g.send :metaclass, 0
      end
      g.send :undef_method, 1
    end
  end

  # TESTED
  class Alias
    def bytecode(g)
      g.push_literal @current
      g.push_literal @new
      g.push :self
      g.send :alias_method, 2, true
    end
  end

  class VAlias
    def bytecode(g)
      g.push_literal @new
      g.push_literal @current
      g.push_cpath_top
      g.find_const :Globals
      g.send :add_alias, 2
    end
  end

  # TESTED
  class Arguments
    def bytecode(g)

      min = @required.size
      if @splat
        max = 1024
      else
        max = min + @optional.size
      end

      g.check_argcount min, max

      @required.each do |var|
        unless var.on_stack?
          g.set_local_from_fp var.slot, var.stack_position
        end
      end

      @optional.each do |var|
        assign = @mapped_defaults[var.name]

        use_passed = g.new_label
        done = g.new_label
        g.passed_arg var.stack_position
        g.git use_passed
        assign.bytecode(g)
        g.pop
        g.goto done

        use_passed.set!
        g.set_local_from_fp var.slot, var.stack_position

        done.set!
      end

      if @splat.kind_of? Local
        g.make_rest_fp @required.size + @optional.size
        lv = LocalAssignment.new(@compiler)
        lv.from_variable @splat
        lv.bytecode(g)
        g.pop
      end

      if @block_arg
        g.push_block
        g.dup
        g.is_nil

        after = g.new_label
        g.git after

        g.push_const :Proc
        g.send :__from_block__, 1

        after.set!

        @block_arg.bytecode(g)
        g.pop
      end
    end
  end

  # TESTED
  class Define

    def argument_info
      [@arguments.arity, @arguments.optional.size]
    end

    def compile_body(g)
      desc = new_description()
      meth = desc.generator

      prelude(g, meth)

      set(:scope, nil) do
        show_errors(meth) do
          @arguments.bytecode(meth)
          desc.run self, @body
        end
      end

      required =  @arguments.required.map {|x| x.name} if @arguments.required
      optional =  @arguments.optional.map {|x| x.name} if @arguments.optional
        
      desc.args = [required, optional, @arguments.splat && @arguments.splat.name]

      meth.sret
      meth.close

      use_plugin g, :method, desc

      return desc
    end

    def bytecode(g)
      g.push_literal compile_body(g)

      if @compiler.kernel?
        g.push :self
        g.add_method @name
      else
        g.push_literal @name
        g.push :self
        g.send :__add_method__, 2
      end
    end
  end

  # TESTED
  class DefineSingleton
    def bytecode(g)
      g.push_literal compile_body(g)
      if @compiler.kernel?
        @object.bytecode(g)
        g.attach_method @name
      else
        g.push_literal @name
        @object.bytecode(g)
        g.send :metaclass, 0
        g.send :attach_method, 2
      end
    end
  end

  # TESTED
  class Range
    def bytecode(g)
      @finish.bytecode(g)
      @start.bytecode(g)
      g.push_cpath_top
      g.find_const :Range
      g.send :new, 2
    end
  end

  # TESTED
  class RangeExclude
    def bytecode(g)
      g.push :true
      @finish.bytecode(g)
      @start.bytecode(g)

      g.push_cpath_top
      g.find_const :Range
      g.send :new, 3
    end
  end

  class CVarAssign
    def bytecode(g)
      @value.bytecode(g)
      g.push_literal @name
      if @in_module
        g.push :self
      else
        g.push_context
      end
      g.send :class_variable_set, 2
    end
  end

  class CVar
    def bytecode(g)
      g.push_literal @name
      if @in_module
        g.push :self
      else
        g.push_context
      end
      g.send :class_variable_get, 1
    end
  end

  class File
    def bytecode(g)
      g.push_context
      g.send :active_path, 0
    end
  end
end
end

