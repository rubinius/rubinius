module Rubinius
  module AST
    class Break < Node
      attr_accessor :value

      def self.from(p, expr)
        node = Break.new p.compiler
        node.value = expr || Nil.from(p)
        node
      end

      def jump_error(g, msg)
        g.push :self
        g.push_const :LocalJumpError
        g.push_literal msg
        g.send :raise, 2, true
      end

      def in_block
        @in_block = true
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

    class Case < Node
      attr_accessor :whens, :else

      def self.from(p, whens, else_body)
        node = Case.new p.compiler
        node.whens = whens
        node.else = else_body || Nil.from(p)
        node
      end

      def children
        @whens.dup << @else
      end

      def bytecode(g)
        done = g.new_label

        @whens.each do |w|
          w.bytecode(g, done)
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
        [@receiver] + @whens << @else
      end

      def bytecode(g)
        done = g.new_label

        @receiver.bytecode(g)

        @whens.each do |w|
          w.receiver_bytecode(g, done)
        end

        g.pop
        @else.bytecode(g)

        done.set!
      end
    end

    class When < Node
      attr_accessor :conditions, :body, :single, :splat

      def self.from(p, conditions, body)
        node = When.new p.compiler
        node.body = body || Nil.from(p)

        if conditions.kind_of? ArrayLiteral
          if conditions.body.last.kind_of? When
            last = conditions.body.pop
            if last.conditions.kind_of? ArrayLiteral
              conditions.body.concat last.conditions.body
            else
              node.splat = SplatWhen.from p, last.conditions
            end
          end

          if conditions.body.size == 1 and !node.splat
            node.single = conditions.body.first
          else
            node.conditions = conditions
          end
        else
          node.conditions = conditions
        end

        node
      end

      def children
        [@conditions, @splat, @body]
      end

      def condition_bytecode(g, condition)
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
    end

    class SplatWhen < Node
      attr_accessor :condition

      def self.from(p, condition)
        node = SplatWhen.new p.compiler
        node.condition = condition
        node
      end

      def receiver_bytecode(g, body, nxt)
        g.dup
        @condition.bytecode(g)
        g.cast_array
        g.swap
        g.send :__matches_when__, 1
        g.git body
      end

      def bytecode(g, body, nxt)
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

    class Match < Node
      attr_accessor :pattern

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

    class Next < Break
      def self.from(p, expr)
        node = Next.new p.compiler
        node.value = expr
        node
      end

      def in_block
        @in_block = true
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
      attr_accessor :value

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

      def in_block
        @in_block = true
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
  end
end
