# -*- encoding: us-ascii -*-

module Rubinius
  module AST
    class Case < Node
      attr_accessor :whens, :else

      def initialize(line, whens, else_body)
        @line = line
        @whens = whens
        @else = else_body || NilLiteral.new(line)
      end

      def bytecode(g)
        pos(g)

        done = g.new_label

        @whens.each do |w|
          w.bytecode(g, done)
        end

        @else.bytecode(g)

        # See command in if about why using line 0
        g.set_line 0

        done.set!
      end

      def receiver_sexp
        nil
      end

      def to_sexp
        else_sexp = @else.kind_of?(NilLiteral) ? nil : @else.to_sexp
        sexp = [:case, receiver_sexp]
        sexp << [:whens] + @whens.map { |x| x.to_sexp }
        sexp << else_sexp
        sexp
      end
    end

    class ReceiverCase < Case
      attr_accessor :receiver

      def initialize(line, receiver, whens, else_body)
        @line = line
        @receiver = receiver
        @whens = whens
        @else = else_body || NilLiteral.new(line)
      end

      def bytecode(g)
        pos(g)

        done = g.new_label

        @receiver.bytecode(g)

        @whens.each do |w|
          w.receiver_bytecode(g, done)
        end

        g.pop
        @else.bytecode(g)

        # See command in if about why using line 0
        g.set_line 0

        done.set!
      end

      def receiver_sexp
        @receiver.to_sexp
      end
    end

    class When < Node
      attr_accessor :conditions, :body, :single, :splat

      def initialize(line, conditions, body)
        @line = line
        @body = body || NilLiteral.new(line)
        @splat = nil
        @single = nil

        if conditions.kind_of? ConcatArgs
          @splat = SplatWhen.new line, conditions.rest
          conditions = conditions.array
        end

        if conditions.kind_of? ArrayLiteral
          if conditions.body.last.kind_of? When
            last = conditions.body.pop
            if last.conditions.kind_of? ArrayLiteral
              conditions.body.concat last.conditions.body
            elsif last.single
              @splat = SplatWhen.new line, last.single
            else
              @splat = SplatWhen.new line, last.conditions
            end
          end

          if conditions.body.size == 1 and !@splat
            @single = conditions.body.first
          else
            @conditions = conditions
          end
        elsif conditions.kind_of? SplatValue
          @splat = SplatWhen.new line, conditions.value
          @conditions = nil
        else
          @conditions = conditions
        end
      end

      def condition_bytecode(g, condition)
        condition.pos(g)
        g.dup
        condition.bytecode(g)
        g.swap
        g.send :===, 1
      end

      def receiver_bytecode(g, done)
        body = g.new_label
        nxt = g.new_label

        if @single
          condition_bytecode(g, @single)
          g.gif nxt
        else
          if @conditions
            @conditions.body.each do |c|
              condition_bytecode(g, c)
              g.git body
            end
          end

          @splat.receiver_bytecode(g, body, nxt) if @splat
          g.goto nxt

          body.set!
        end

        g.pop
        @body.bytecode(g)
        g.goto done

        nxt.set!
      end

      def bytecode(g, done)
        pos(g)

        nxt = g.new_label
        body = g.new_label

        if @single
          @single.bytecode(g)
          g.gif nxt
        else
          if @conditions
            @conditions.body.each do |condition|
              condition.bytecode(g)
              g.git body
            end
          end

          @splat.bytecode(g, body, nxt) if @splat
          g.goto nxt

          body.set!
        end

        @body.bytecode(g)
        g.goto done

        nxt.set!
      end

      def to_sexp
        if @single
          conditions_sexp = [:array, @single.to_sexp]
        else
          conditions_sexp = @conditions ? @conditions.to_sexp : []
          conditions_sexp << @splat.to_sexp if @splat
        end
        [:when, conditions_sexp, @body.to_sexp]
      end
    end

    class SplatWhen < Node
      attr_accessor :condition

      def initialize(line, condition)
        @line = line
        @condition = condition
      end

      def receiver_bytecode(g, body, nxt)
        pos(g)

        g.dup
        @condition.bytecode(g)
        g.cast_array
        g.push_literal Rubinius::Compiler::Runtime
        g.rotate(3)
        g.send :matches_when, 2
        g.git body
      end

      def bytecode(g, body, nxt)
        # TODO: why is this empty?
      end

      def to_sexp
        [:when, @condition.to_sexp, nil]
      end
    end

    class Flip2 < Node
      def initialize(line, start, finish)
        @line = line
        @start = start
        @finish = finish
      end

      def sexp_name
        :flip2
      end

      def exclusive?
        false
      end

      def bytecode(g)
        done = g.new_label
        on_label = g.new_label
        index = g.state.flip_flops
        g.state.push_flip_flop

        get_flip_flop(g, index)
        g.git on_label

        @start.bytecode(g)
        g.dup
        g.gif done
        g.pop
        set_flip_flop(g, index, true)

        if exclusive?
          g.goto done
        else
          g.pop
        end

        on_label.set!
        g.push_literal true
        @finish.bytecode(g)
        g.gif done
        set_flip_flop(g, index, false)
        g.pop

        done.set!
      end

      def get_flip_flop(g, index)
        g.push_literal Rubinius::Compiler::Runtime
        g.push_scope
        g.push_literal index
        g.send(:get_flip_flop, 2)
      end

      def set_flip_flop(g, index, value)
        g.push_literal Rubinius::Compiler::Runtime
        g.push_scope
        g.push_literal index
        g.push_literal value
        g.send(:set_flip_flop, 3)
      end

      def to_sexp
        [sexp_name, @start.to_sexp, @finish.to_sexp]
      end
    end

    class Flip3 < Flip2
      def sexp_name
        :flip3
      end

      def exclusive?
        true
      end
    end

    class If < Node
      attr_accessor :condition, :body, :else

      def initialize(line, condition, body, else_body)
        @line = line
        @condition = condition
        @body = body || NilLiteral.new(line)
        @else = else_body || NilLiteral.new(line)
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

        # Use line 0 to indicate "compiler generated code"
        # so that debuggers and such don't get confused by
        # thinking the then branch jumps into the else branch.
        g.set_line 0
        done.set!
      end

      def to_sexp
        else_sexp = @else.kind_of?(NilLiteral) ? nil : @else.to_sexp
        [:if, @condition.to_sexp, @body.to_sexp, else_sexp]
      end
    end

    class While < Node
      attr_accessor :condition, :body, :check_first

      def initialize(line, condition, body, check_first)
        @line = line
        @condition = condition
        @body = body || NilLiteral.new(line)
        @check_first = check_first
      end

      def condition_bytecode(g, bottom, use_gif)
        @condition.bytecode(g)
        if use_gif
          g.gif bottom
        else
          g.git bottom
        end
      end

      def body_bytecode(g, lbl)
        g.state.push_loop
        @body.bytecode(g)
        g.state.pop_loop

        # This is a loop epilogue. Nothing that changes
        # computation should be put here.
        lbl.set!
        g.pop
        g.check_interrupts
      end

      def bytecode(g, use_gif=true)
        pos(g)

        g.push_modifiers

        top = g.new_label
        post = g.next = g.new_label
        bottom = g.new_label

        g.break = g.new_label

        if @check_first
          g.redo = g.new_label

          top.set!
          condition_bytecode(g, bottom, use_gif)

          g.redo.set!
          body_bytecode(g, post)
        else
          g.redo = top

          top.set!
          body_bytecode(g, post)

          condition_bytecode(g, bottom, use_gif)
        end

        g.goto top

        # See other set_line(0) comments
        g.set_line 0

        bottom.set!
        g.push :nil
        g.break.set!

        g.pop_modifiers
      end

      def sexp_name
        :while
      end

      def to_sexp
        [sexp_name, @condition.to_sexp, @body.to_sexp, @check_first]
      end
    end

    class Until < While
      def bytecode(g)
        super(g, false)
      end

      def sexp_name
        :until
      end
    end

    class Match < Node
      attr_accessor :pattern

      def initialize(line, pattern, flags)
        @line = line
        @pattern = RegexLiteral.new line, pattern, flags
      end

      def bytecode(g)
        pos(g)

        g.push_rubinius
        g.find_const :Globals
        g.push_literal :$_
        g.send :[], 1

        @pattern.bytecode(g)

        g.send :=~, 1
      end

      def to_sexp
        [:match, @pattern.to_sexp]
      end
    end

    class Match2 < Node
      attr_accessor :pattern, :value

      def initialize(line, pattern, value)
        @line = line
        @pattern = pattern
        @value = value
      end

      def bytecode(g)
        pos(g)

        @pattern.bytecode(g)
        @value.bytecode(g)
        g.send :=~, 1
        if @pattern.kind_of? RegexLiteral
          regexp = Regexp.new(@pattern.source)
          if table = regexp.name_table
            table.sort_by { |name, idx| idx }.each do |name, idx|
              local = g.state.scope.new_local name
              g.last_match 5, idx.last - 1

              case local
              when Compiler::LocalVariable
                g.set_local local.slot
              when Compiler::EvalLocalVariable
                g.push_variables
                g.swap
                g.push_literal name
                g.swap
                g.send :set_eval_local, 2, false
              else
                raise CompileError, "unknown type of local #{local.inspect}"
              end

              g.pop
            end
          end
        end
      end

      def to_sexp
        [:match2, @pattern.to_sexp, @value.to_sexp]
      end
    end

    class Match3 < Node
      attr_accessor :pattern, :value

      def initialize(line, pattern, value)
        @line = line
        @pattern = pattern
        @value = value
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)
        @pattern.bytecode(g)
        g.send :=~, 1
      end

      def to_sexp
        [:match3, @pattern.to_sexp, @value.to_sexp]
      end
    end

    class Break < Node
      attr_accessor :value

      def initialize(line, expr)
        @line = line
        @value = expr || NilLiteral.new(line)
      end

      def jump_error(g, name)
        g.push_rubinius
        g.push_literal name
        g.send :jump_error, 1
      end

      def bytecode(g)
        pos(g)

        @value.bytecode(g)

        if g.break
          g.goto g.break
        elsif g.state.block?
          g.raise_break
        else
          g.pop
          jump_error g, :break
        end
      end

      def sexp_name
        :break
      end

      def to_sexp
        sexp = [sexp_name]
        sexp << @value.to_sexp if @value
        sexp
      end
    end

    class Next < Break
      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g)
        pos(g)

        # From "The Ruby Programming Lanuage"
        #  "When next is used in a loop, any values following the next
        #   are ignored"
        #
        # By ignored, it must mean evaluated and the value of the expression
        # is thrown away, because 1.8 evaluates them even though it doesn't
        # use them.
        if @value
          @value.bytecode(g)
        else
          g.push :nil
        end

        if g.state.loop?
          g.goto g.next
        elsif g.state.block?
          if g.next
            g.goto g.next
          else
            g.ret
          end
        else
          g.pop

          jump_error g, :next
        end
      end

      def sexp_name
        :next
      end
    end

    class Redo < Break
      def initialize(line)
        @line = line
      end

      def bytecode(g)
        pos(g)

        if g.redo
          g.check_interrupts
          g.goto g.redo
        else
          jump_error g, :redo
        end
      end

      def to_sexp
        [:redo]
      end
    end

    class Retry < Break
      def initialize(line)
        @line = line
      end

      def bytecode(g)
        pos(g)

        if g.retry
          g.goto g.retry
        else
          jump_error g, :retry
        end
      end

      def to_sexp
        [:retry]
      end
    end

    class Return < Node
      attr_accessor :value

      def initialize(line, expr)
        @line = line
        @value = expr
        @splat = nil
      end

      def block=(node)
        @value.block = node if @value
      end

      def bytecode(g, force=false)
        pos(g)

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

        if lcl = g.state.rescue?
          g.push_stack_local lcl
          g.restore_exception_state
        end

        if g.state.block?
          g.raise_return
        elsif !force and g.state.ensure?
          g.ensure_return
        else
          g.ret
        end
      end

      def to_sexp
        sexp = [:return]
        sexp << @value.to_sexp if @value
        sexp << @splat.to_sexp if @splat
        sexp
      end
    end
  end
end
