module Rubinius
  module AST
    class Begin < Node
      attr_accessor :rescue

      def initialize(line, body)
        @line = line
        @rescue = body
      end

      def bytecode(g)
        @rescue.bytecode(g)
      end

      def to_sexp
        @rescue.to_sexp
      end
    end

    EnsureType = 1

    class Ensure < Node
      attr_accessor :body, :ensure

      def initialize(line, body, ensr)
        @line = line
        @body = body || NilLiteral.new(line)
        @ensure = ensr
      end

      def bytecode(g)
        pos(g)

        ok = g.new_label
        ex = g.new_label
        g.setup_unwind ex, EnsureType

        # TODO: ?
        g.new_label.set!

        g.push_exception_state
        outer_exc_state = g.new_stack_local
        g.set_stack_local outer_exc_state
        g.pop

        old_break = g.break
        new_break = g.new_label
        g.break = new_break

        g.state.push_ensure
        @body.bytecode(g)
        g.state.pop_ensure

        g.break = old_break

        g.pop_unwind
        g.goto ok

        check_break = nil

        if new_break.used?
          used_break_local = g.new_stack_local
          check_break = g.new_label

          new_break.set!
          g.pop_unwind

          g.push :true
          g.set_stack_local used_break_local
          g.pop

          g.goto check_break
        end

        ex.set!

        g.push_exception_state

        g.state.push_rescue(outer_exc_state)
        @ensure.bytecode(g)
        g.state.pop_rescue
        g.pop

        g.restore_exception_state

        # Re-raise the exception
        g.reraise

        ok.set!

        if check_break
          g.push :false
          g.set_stack_local used_break_local
          g.pop

          check_break.set!
        end

        # Now, re-emit the code for the ensure which will run if there was no
        # exception generated.
        @ensure.bytecode(g)
        g.pop

        if check_break
          post = g.new_label

          g.push_stack_local used_break_local
          g.gif post

          if g.break
            g.goto g.break
          else
            g.raise_break
          end

          post.set!
        end
      end

      def to_sexp
        [:ensure, @body.to_sexp, @ensure.to_sexp]
      end
    end

    RescueType = 0

    class Rescue < Node
      attr_accessor :body, :rescue, :else

      def initialize(line, body, rescue_body, else_body)
        @line = line
        @body = body
        @rescue = rescue_body
        @else = else_body
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
          outer_retry = g.retry

          this_retry = g.new_label
          reraise = g.new_label
          els     = g.new_label
          done    = g.new_label

          # Save the current exception into a stack local
          g.push_exception_state
          outer_exc_state = g.new_stack_local
          g.set_stack_local outer_exc_state
          g.pop

          this_retry.set!
          ex = g.new_label
          g.setup_unwind ex, RescueType

          # TODO: ?
          g.new_label.set!

          if current_break = g.break
            # Make a break available to use, which we'll use to
            # lazily generate a cleanup area
            g.break = g.new_label
          end

          # Use a lazy label to patch up prematuraly leaving a begin
          # body via retry.
          if outer_retry
            g.retry = g.new_label
          end

          # Also handle redo unwinding through the rescue
          if current_redo = g.redo
            g.redo = g.new_label
          end

          @body.bytecode(g)
          g.pop_unwind
          g.goto els

          if current_break
            if g.break.used?
              g.break.set!
              g.pop_unwind

              # Reset the outer exception
              g.push_stack_local outer_exc_state
              g.restore_exception_state

              g.goto current_break
            end

            g.break = current_break
          end

          if current_redo
            if g.redo.used?
              g.redo.set!
              g.pop_unwind

              # Reset the outer exception
              g.push_stack_local outer_exc_state
              g.restore_exception_state

              g.goto current_redo
            end

            g.redo = current_redo
          end

          if outer_retry
            if g.retry.used?
              g.retry.set!
              g.pop_unwind

              # Reset the outer exception
              g.push_stack_local outer_exc_state
              g.restore_exception_state

              g.goto outer_retry
            end

            g.retry = outer_retry
          end

          # We jump here if an exception has occured in the body
          ex.set!

          # Expose the retry label here only, not before this.
          g.retry = this_retry

          # Save exception state to use in reraise
          g.push_exception_state

          raised_exc_state = g.new_stack_local
          g.set_stack_local raised_exc_state
          g.pop

          # Save the current exception, so that calling #=== can't trample
          # it.
          g.push_current_exception

          @rescue.bytecode(g, reraise, done, outer_exc_state)
          reraise.set!

          # Restore the exception state we saved and the reraise. The act
          # of checking if an exception matches can run any code, which
          # can easily trample on the current exception.
          #
          # Remove the direct exception so we can get to the state
          g.pop

          # Restore the state and reraise
          g.push_stack_local raised_exc_state
          g.restore_exception_state
          g.reraise

          els.set!
          if @else
            g.pop
            @else.bytecode(g)
          end

          done.set!

          g.push_stack_local outer_exc_state
          g.restore_exception_state
        end
        g.pop_modifiers
      end

      def to_sexp
        sexp = [:rescue, @body.to_sexp, @rescue.to_sexp]
        sexp << @else.to_sexp if @else
        sexp
      end
    end

    class RescueCondition < Node
      attr_accessor :conditions, :assignment, :body, :next, :splat

      def initialize(line, conditions, body, nxt)
        @line = line
        @next = nxt
        @splat = nil
        @assignment = nil

        case conditions
        when ArrayLiteral
          @conditions = conditions
        when ConcatArgs
          @conditions = conditions.array
          @splat = RescueSplat.new line, conditions.rest
        when SplatValue
          @splat = RescueSplat.new line, conditions.value
        when nil
          condition = ConstantAccess.new line, :StandardError
          @conditions = ArrayLiteral.new line, [condition]
        end

        case body
        when Block
          @assignment = body.array.shift if assignment? body.array.first
          @body = body
        when nil
          @body = NilLiteral.new line
        else
          if assignment? body
            @assignment = body
            @body = NilLiteral.new line
          else
            @body = body
          end
        end
      end

      def assignment?(node)
        case node
        when VariableAssignment
          value = node.value
        when AttributeAssignment
          value = node.arguments.array.last
        else
          return false
        end

        return true if value.kind_of? GlobalVariableAccess and value.name == :$!
      end

      def bytecode(g, reraise, done, outer_exc_state)
        pos(g)
        body = g.new_label

        # Exception has left the current exception on the top
        # of the stack, use it rather than using push_current_exception.

        if @conditions
          @conditions.body.each do |c|
            g.dup
            c.bytecode(g)
            g.swap
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

        # Remove the current exception from the top of the stack now
        # that we've hit a handler
        g.pop

        if @assignment
          @assignment.bytecode(g)
          g.pop
        end

        current_break = g.break
        g.break = g.new_label

        g.state.push_rescue(outer_exc_state)
        @body.bytecode(g)
        g.state.pop_rescue

        g.clear_exception
        g.goto done

        if g.break.used?
          g.break.set!
          g.clear_exception

          # Reset the outer exception
          g.push_stack_local outer_exc_state
          g.restore_exception_state

          if current_break
            g.goto current_break
          else
            g.raise_break
          end
        end

        g.break = current_break
        if @next
          if_false.set!
          @next.bytecode(g, reraise, done, outer_exc_state)
        end
      end

      def to_sexp
        array = @conditions.to_sexp
        array << @assignment.to_sexp if @assignment
        array << @splat.to_sexp if @splat

        sexp = [:resbody, array]
        case @body
        when Block
          sexp << (@body ? @body.array.map { |x| x.to_sexp } : nil)
        when nil
          sexp << nil
        else
          sexp << @body.to_sexp
        end

        sexp << @next.to_sexp if @next

        sexp
      end
    end

    class RescueSplat < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
      end

      def bytecode(g, body)
        pos(g)

        g.dup
        @value.bytecode(g)
        g.cast_array
        g.swap
        g.send :__rescue_match__, 1
        g.git body
      end

      def to_sexp
        [:splat, @value.to_sexp]
      end
    end
  end
end
