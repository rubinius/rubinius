module Rubinius
  module AST
    class And < Node
      attr_accessor :left, :right

      def initialize(line, left, right)
        @line = line
        @left = left
        @right = right
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

      def defined(g)
        t = g.new_label
        f = g.new_label
        done = g.new_label

        case @left
        when GlobalVariableAccess, InstanceVariableAccess
          g.goto t
        else
          @left.value_defined(g, f)
          g.pop
        end

        case @right
        when GlobalVariableAccess, InstanceVariableAccess
          g.goto t
        else
          @right.value_defined(g, f)
          g.pop
        end

        t.set!
        g.push_literal "expression"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def sexp_name
        :and
      end

      def to_sexp
        [sexp_name, @left.to_sexp, @right.to_sexp]
      end
    end

    class Or < And
      def bytecode(g)
        super(g, false)
      end

      def sexp_name
        :or
      end
    end

    class Not < Node
      attr_accessor :value

      def initialize(line, value)
        @line = line
        @value = value
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

      def defined(g)
        t = g.new_label
        f = g.new_label
        done = g.new_label

        case @value
        when GlobalVariableAccess, InstanceVariableAccess
          g.goto t
        else
          @value.value_defined(g, f)
          g.pop
        end

        t.set!
        g.push_literal "expression"
        g.goto done

        f.set!
        g.push :nil

        done.set!
      end

      def to_sexp
        [:not, @value.to_sexp]
      end
    end

    class OpAssign1 < Node
      attr_accessor :receiver, :op, :index, :value

      def initialize(line, receiver, index, op, value)
        @line = line
        @receiver = receiver
        @op = op
        @index = index.body
        @value = value
      end

      def bytecode(g)
        pos(g)

        # X: Snippet used for explanation: h[:a] += 3
        # X: given h = { :a => 2 }
        # X: Pull h onto the stack
        @receiver.bytecode(g)
        # X: Pull :a in
        @index.each do |idx|
          idx.bytecode(g)
        end

        recv_stack = @index.size + 1

        # Dup the receiver and index to use later
        g.dup_many recv_stack

        #
        # X: Call [](:a) on h
        #
        # @index.size will be 1

        g.send :[], @index.size

        # X: 2 is now on the top of the stack (TOS)

        # A special case, where we use the value as boolean
        if @op == :or or @op == :and
          fnd = g.new_label
          fin = g.new_label

          # We dup the value from [] to leave it as the value of the
          # expression

          g.dup
          if @op == :or
            g.git fnd
          else
            g.gif fnd
          end

          # Ok, take the extra copy off and pull the value onto the stack
          g.pop

          # The receiver and index are still on the stack

          @value.bytecode(g)

          # retain the rhs as the expression value
          g.dup
          g.move_down recv_stack + 1

          g.send :[]=, @index.size + 1
          g.pop

          # Leaves the value we moved down the stack on the top
          g.goto fin

          fnd.set!

          # Clean up the stack but retain return value from :[]
          g.move_down recv_stack
          g.pop_many recv_stack

          fin.set!
        else
          # @op is something like + or -
          # We pull in @value to the stack
          @value.bytecode(g)
          # X: 3 TOS

          # ... then call it as an argument to @or, called on the return
          # from [].
          # X: 2 + 3

          g.send @op, 1
          # X: 5 TOS

          # The new value is on the stack now. It is the last argument to the call
          # to []= because your dupd versions of recv and index are still on the stack.

          # retain the rhs as the expression value
          g.dup
          g.move_down recv_stack + 1

          # X: Call []=(:a, 5) on h
          g.send :[]=, @index.size + 1
          g.pop
        end
      end

      def to_sexp
        index = @index.inject([:arglist]) { |s, x| s << x.to_sexp }
        op = @op == :or ? :"||" : :"&&"
        [:op_asgn1, @receiver.to_sexp, index, op, @value.to_sexp]
      end
    end

    class OpAssign2 < Node
      attr_accessor :receiver, :name, :assign, :op, :value

      def initialize(line, receiver, name, op, value)
        @line = line
        @receiver = receiver
        @name = name
        @op = op
        @value = value
        @assign = name.to_s[-1] == ?= ? name : :"#{name}="
      end

      def bytecode(g)
        pos(g)

        # X: h[:a] += 3, given h.a == 2
        @receiver.bytecode(g)
        # X: TOS = h
        g.dup
        g.send @name, 0
        # X: TOS = 2

        if @op == :or or @op == :and
          fnd = g.new_label
          fin = g.new_label

          g.dup
          if @op == :or
            g.git fnd
          else
            g.gif fnd
          end

          # Remove the copy of 2 and push @value on the stack
          g.pop
          @value.bytecode(g)

          # Retain the this value to use as the expression value
          g.dup
          g.move_down 2

          # Call the assignement method, passing @value as the argument
          g.send @assign, 1
          g.pop

          g.goto fin

          fnd.set!

          # Clean up the stack
          g.swap
          g.pop

          fin.set!
        else
          @value.bytecode(g)
          # X: TOS = 3
          # X: 2 + 3
          g.send @op, 1

          # Retain the this value to use as the expression value
          g.dup
          g.move_down 2
          # X: TOS = 5
          g.send @assign, 1
          # X: TOS = 5 (or whatever a=() returns)

          # Discard the methods return value
          g.pop
        end
      end

      def to_sexp
        op = @op == :or ? :"||" : :"&&"
        [:op_asgn2, @receiver.to_sexp, :"#{@name}=", op, @value.to_sexp]
      end
    end

    class OpAssignAnd < Node
      attr_accessor :left, :right

      def initialize(line, left, right)
        @line = line
        @left = left
        @right = right
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

      def defined(g)
        g.push_literal "assignment"
      end

      def sexp_name
        :op_asgn_and
      end

      def to_sexp
        [sexp_name, @left.to_sexp, @right.to_sexp]
      end
    end

    class OpAssignOr < OpAssignAnd
      def bytecode(g)
        pos(g)

        @left.or_bytecode(g) do
          @right.bytecode(g)
        end
      end

      def sexp_name
        :op_asgn_or
      end
    end
  end
end
