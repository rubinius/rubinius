module Rubinius
  module AST
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

    class Ensure < Node
      attr_accessor :body, :ensure

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

    class Rescue < Node
      attr_accessor :body, :rescue, :else

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
  end
end
