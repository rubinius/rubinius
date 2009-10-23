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
    end

    class Or < And
      def bytecode(g)
        super(g, false)
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
        g.dup
        # X: Pull :a in
        @index.each do |idx|
          idx.bytecode(g)
        end
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

          # Pull the index onto the stack again, swaping to push
          # @value to the top of the stack (last argument)
          @index.each do |idx|
            idx.bytecode(g)
          end

          @value.bytecode(g)

          # retain the rhs as the expression value
          g.dup
          g.move_down @index.size + 2

          g.send :[]=, @index.size + 1
          g.pop
          g.goto fin

          fnd.set!

          # Clean up the stack by swaping, allowing us to pop of the extra
          # copy of @object we left on the stack
          g.swap
          g.pop

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

          # The new value is on the stack now. It needs to be the last argument
          # when we call []=, so we call the index again, swapping each time to
          # move the new value up to the top of the stack (where the last
          # argument lives)
          @index.each do |idx|
            idx.bytecode(g)
            g.swap
          end

          # retain the rhs as the expression value
          g.dup
          g.move_down @index.size + 2

          # X: Call []=(:a, 5) on h
          g.send :[]=, @index.size + 1
          g.pop
        end
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
    end

    class OpAssignOr < OpAssignAnd
      def bytecode(g)
        pos(g)

        @left.or_bytecode(g) do
          @right.bytecode(g)
        end
      end
    end
  end
end
