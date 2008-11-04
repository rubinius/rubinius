require 'compiler/node'

class Compiler
  class Node

    # forward decls - order is layered, and sorted in each layer
    class StringLiteral < Node;          end

    class ClosedScope < Node;            end
    class DynamicString < StringLiteral; end
    class MethodCall < Node;             end
    class Snippit < ClosedScope;         end

    class ArrayLiteral < Node;           end
    class Call < MethodCall;             end
    class DynamicArguments < Node;       end
    class DynamicRegex < DynamicString;  end
    class Expression < Snippit;          end
    class Iter < Node;                   end
    class LocalVariable < Node;          end
    class NumberLiteral < Node;          end
    class While < Node;                  end

    ############################################################
    # Start of Node subclasses

    class AccessSlot < Node
      def args(idx)
        @index = idx
      end

      attr_reader :index
    end

    class Alias < Node
      kind :alias

      def args(current, name)
        if current.kind_of? Literal
          current = current.value
          name = name.value
        end

        @current, @new = current, name
      end

      attr_accessor :current, :new
    end

    # And represents either +and+ or +&&+.
    # The precedence difference between the
    # two has been resolved by the parser so
    # both types map into this one node (but
    # their child and parent nodes could be
    # different.)
    #
    # It contains both the left and the right
    # subexpression.
    #
    # Sexp tag: +:and+
    #
    # Example:
    #
    #   foo and bar
    #       ^^^
    #
    #   baz && quux
    #       ^^
    #
    class And < Node
      kind :and

      def args(left, right)
        @left, @right = left, right
      end

      attr_accessor :left, :right
    end

    class ArgList < ArrayLiteral
      kind :arglist
    end

    # +Arguments+ is for the representation of a method _definition's_
    # argument list. It contains four sub-nodes, one each for required
    # args, optional args, splat name and default value expressions.
    #
    # There are a few interesting details: because default values can
    # be any expressions (try it, you can use a block, a class, define
    # another function etc.), the entire expression subtree is stored.
    # The optional arguments are just the variable names, and the values
    # are first processed into their own little AST subtrees which are
    # then stored in a mapping whence they may be executed at runtime
    # if the default value is needed.
    #
    # The Arguments node performs quite a bit of other normalisation
    # also, as well as coordinating setting up the arguments as local
    # variables with the appropriate LocalScope and providing the
    # arity calculation for this method. See further documentation
    # about those operations in the individual methods of Arity.
    #
    # Sexp tag: +:args+
    #
    # Example:
    #
    #   def foo(a, b = lambda { :moo }, *c, &d)
    #           ^^^^^^^^^^^^^^^^^^^^^^^^^^
    #             These go in the node.
    #     ...
    #   end
    #
    class Arguments < Node
      kind :args

      # [[:obj], [], nil, nil]
      # required, optional, splat, defaults
      def consume(sexp)
        req, opt, splat, block, defaults = [], [], nil, nil, []
        opt_block = sexp.block(true) rescue nil # HACK rescue
        opt_block.shift if opt_block # :block

        sexp.reverse.each do |s|
          case s.to_s
          when /^&(.*)/ then
            block = $1.to_sym
          when /^\*(.*)/ then
            splat = $1.empty? ? :@anon_splat  : $1.to_sym # TODO: don't like
          else
            if opt_block then
              defaults.unshift opt_block.pop
              opt.unshift s
              opt_block = nil if opt_block.empty?
            else
              req.unshift s
            end
          end
        end

        defaults = nil if defaults.empty?

        [req, opt, splat, block, defaults]
      end

      def args(req, opt, splat, block, defaults)
        @mapped_defaults = {}
        @required, @optional, @splat, @block_arg, @defaults =
          req, opt, splat, block, defaults

        i = 0
        scope = get(:scope)

        @required.map! do |var|
          var, depth = scope.find_local(var)
          var.argument!(i)
          i += 1
          var
        end

        @optional.map! do |var|
          var, depth = scope.find_local(var)
          var.argument!(i, true)
          i += 1
          var
        end

        if @splat.kind_of? Symbol
          var, depth = scope.find_local(@splat)
          var.argument!(i, true)
          @splat = var
        end

        @block_arg = convert(s(:block_arg, @block_arg)) if @block_arg

        if @defaults
          @defaults.map! { |node| convert(node) }

          @defaults.each do |x|
            @mapped_defaults[x.name] = x
          end
        end
      end

      attr_accessor :required, :optional, :splat, :defaults, :block_arg

      def arity
        return @required.size
      end
    end

    class ArrayLiteral < Node
      kind :array

      # We do this to get around having to do *body in
      # args. 1024 is the max number of args, so if an
      # array had more elements that than, args would
      # get an args error. This lets us leave it as an
      # Array.
      def consume(sexp)
        sexp.map! { |s| convert(s) }
        [sexp]
      end

      def args(body)
        @body = body
      end

      attr_accessor :body
    end

    class AttrAssign < Call
      kind :attrasgn

      def args(obj, meth, args=nil)
        @in_masgn = false
        @object, @method = obj, meth
        @arguments = args
        @rhs_expression = nil

        # Strange. nil is passed when it's self. Whatevs.
        @object ||= Self.new @compiler

        if @method.to_s[-1] != ?=
          @method = "#{@method}=".to_sym
        end

        collapse_args()

        # NOTE: []= has special semantics. It can be passed any number of arguments,
        # NOTE: PushArgs is for syntax h[*s] = 3, we have to handle it special

        # When we're in an masgn, there are no arguments
        return unless @arguments

        if @arguments.kind_of? Array
          @rhs_expression = @arguments.pop
        elsif @arguments.is? Splat
raise "no"
          @rhs_expression = nil
        else
          raise Error, "unknown argument form to attrassgn"
        end
      end

      def optional
        []
      end

      def required
        ["#{@object}.#{@method}"]
      end
    end

    # BackRef is any one of the predefined (thread-) global variables
    # that are set after each regexp match operation, except the numbered
    # ones. A BackRef can be $`, $', $& etc. The second character is
    # stored to create the entire variable. See also NthRef.
    #
    # Sexp tag: +:back_ref+
    #
    # Example:
    #
    #   /fo(o)/ =~ variable
    #
    #   puts $`
    #        ^^
    #
    class BackRef < Node
      kind :back_ref

      def args(kind)
        @kind = kind.to_sym
      end

      attr_accessor :kind
    end

    class Begin < Node
      kind :begin

      def args(body)
        @body = body
      end

      attr_accessor :body
    end

    # +Block+ is a special node: it is part of Ruby's semantics rather than
    # syntax. Notably, a Block is NOT a Ruby block (a Proc object.) A Block
    # simply encapsulates multiple expressions into a group. For example, an
    # +if+ expression that has a single line inside it does not have a Block,
    # but one that has two lines will have both of those lines encapsulated
    # inside a Block. It has no fancy purpose apart from grouping.
    #
    # Sexp tag: +:block+
    #
    # Example:
    #
    #   if foo
    #     bar    <
    #     baz    < These will all be inside the :block
    #     quux   <
    #   end
    #
    # Sexp from example (Newline nodes omitted):
    #
    #   [:if,
    #    [:call, nil, :foo, [:arglist]],
    #    [:block ,
    #     [:call, nil, :bar, [:arglist]],
    #     [:call, nil, :baz, [:arglist]],
    #     [:call, nil, :quux, [:arglist]]]]
    #
    class Block < Node
      kind :block

      def args(*body)
        @body = body
      end

      attr_accessor :body
    end

    class BlockAsArgument < Node
      kind :block_arg

      def args(name, position=nil)
        @name = name

        scope = get(:scope)

        @variable, @depth = scope.find_local name
        @variable.in_locals!
      end

      attr_accessor :name, :variable, :depth
    end

    class BlockPass < Node
      kind :block_pass

      def args blk
        @block = blk
      end

      attr_accessor :block
    end

    class Break < Node
      kind :break
      def args(value=nil)
        @value = value

        if @in_block = get(:iter)
          @check_var, _ = get(:scope).find_local :@lre
        end
      end

      attr_accessor :value, :in_block
    end

    class CVar < Node
      kind :cvar

      def args(name)
        @name = name
        @in_module = get(:scope).module_body?
      end

      attr_accessor :name
    end

    class CVarAssign < Node
      kind :cvasgn

      def args(name, val)
        @name, @value = name, val
        @in_module = get(:scope).module_body?
      end

      attr_accessor :name, :value
    end

    class CVarDeclare < CVarAssign
      kind :cvdecl
    end

    class Call < MethodCall
      kind :call

      def inspect
        name = self.class.name.split(/::/).last
        args = Array(@body).map { |o| o.inspect }.join(", ")

        "#{name}[#{@method.inspect}, #{args}]"
      end

      # Args could be an array, splat or argscat
      def collapse_args
        # HACK handle eval wanting to inject locals
        @local_rewrite = nil

        @in_block = get(:iter)

        return unless @arguments

        @arguments = @arguments.body

        @block   = @arguments.grep(BlockPass).first
        @arguments.reject! { |o| BlockPass === o } if @block

        @argcount = @arguments.size
      end

      def args(object, meth, args=nil)
        @object, @method, @arguments = object, meth, args

        collapse_args

        # HACK handle eval wanting to inject locals
        if !object and get(:eval)
          scope = get(:scope)

          if get(:iter)
            var, dep = scope.find_local meth, true, false
          else
            var, dep = scope.find_local meth, false, false
          end

          if var
            @local_rewrite = LocalAccess.new(@compiler)
            @local_rewrite.from_variable(var, dep)
          end
        end
      end

      attr_accessor :object, :method, :arguments
      attr_reader :in_block
      attr_writer :argcount

      def no_args?
        @arguments.nil? or @arguments.empty?
      end

      def static_args?
        return false unless @arguments
        return ! (@arguments[0].kind_of?(Splat)  or
                  @arguments[-2].kind_of?(Splat) or
                  @arguments[-1].kind_of?(Splat) or
                  @arguments[-1].kind_of?(BlockPass))
      end

      def call?
        true
      end

      def argcount
        if @arguments.nil?
          return 0
        elsif @arguments.kind_of? Array
          return @arguments.size
        end

        return nil
      end
    end

    class Case < Node
      kind :case

      def consume(sexp)
        els = nil
        # Detect PT format
        if sexp[1][0] == :when
          i = 1
          whens = []
          while sexp[i].kind_of? Array and sexp[i].first == :when
            whens << convert(sexp[i])
            i += 1
          end

          els = convert(sexp.last)
        else
          whens = sexp[1].map do |w|
            convert(w)
          end
          els = convert(sexp[2])
        end
        [convert(sexp[0]), whens, els]
      end

      def args(recv, whens, els)
        @receiver, @whens, @else = recv, whens, els
      end

      attr_accessor :receiver, :whens, :else
    end

    class Class < ClosedScope
      kind :class

      def args(name, parent, sup, body)
        @name, @parent, @superclass, @body = name, parent, sup, body
      end

      def consume(sexp)
        if sexp[0].kind_of? Symbol
          sym = sexp[0]
          name = nil
        else
          name = convert(sexp[0])
          sym = name.name
        end

        if !name or name.is? ConstFind
          parent = nil
        else
          parent = name.parent
        end

        # We have to set this before converting the body, because
        # we'll need to know it to use find_ivar_index properly.
        # TODO - Still necessary?
        sup = sexp[1]
        if sup and sup[0] == :const
          @superclass_name = sup[1]
        else
          @superclass_name = nil
        end
        @name = sym

        @namespace = get(:namespace)

        body = set(:family => self, :namespace => sym) do
          super([sexp[2]])
        end

        [sym, parent, convert(sexp[1]), body]
      end

      attr_accessor :name, :parent, :superclass, :body

      def module_body?
        true
      end
    end

    # ClosedScope is a metanode in that it does not exist in Ruby code;
    # it merely abstracts common functionality of various real Ruby nodes
    # that must control a local variable scope. These classes will exist
    # as subclasses of ClosedScope below.
    #
    # Most notably, LocalScope objects are kept to maintain a hierarchy
    # of visibility and availability for the runtime.
    class ClosedScope < Node
      def initialize(comp)
        super(comp)

        @use_eval = false
        @alloca = 0
        @top_scope = create_scope()
        @block_scope = []
        @all_scopes = [@top_scope]
        @slot = 0
        @ivar_as_slot = {}
        @visibility = :public
      end

      attr_accessor :visibility

      def create_scope
        LocalScope.new(self)
      end

      attr_accessor :use_eval

      def locals
        @top_scope
      end

      def name
        nil
      end

      attr_reader :outer_scope

      def consume(sexp, iter=false, eval=false)
        @outer_scope = get(:scope)

        set(:scope => self, :iter => iter, :eval => eval) do
          out = convert(sexp[0])
          @all_scopes.each do |scope|
            scope.formalize!
          end
          out
        end
      end

      def depth
        @block_scope.size
      end

      def new_block_scope
        begin
          scope = create_scope()
          @block_scope << scope
          @all_scopes << scope
          yield
        ensure
          @block_scope.pop
        end

        return scope
      end

      def find_local(name, in_block=false, allocate=true)
        # If the caller is not in a block, things can only
        # be in the top_context. Easy enough, return out of the Hash
        # (which might created in for us automatically)
        unless in_block
          if allocate or @top_scope.key?(name)
            return [@top_scope[name], nil]
          else
            return nil
          end
        end

        # They're asking from inside a block, look in the current
        # block scopes as well as top_scope

        if @block_scope.empty?
          raise Error, "You can't be in a block, there are no block scopes"
        end

        lcl = nil
        depth = nil
        dep = 0

        @block_scope.reverse_each do |scope|
          if scope.key?(name)
            if scope.from_eval
              depth = dep + 1
            else
              depth = dep
            end
            lcl = scope[name]
            break
          end
          dep += 1
        end

        # Not found in an outstanding block scope, look in the
        # top context.
        unless lcl
          if @top_scope.key?(name)
            lcl = @top_scope[name]
          elsif !allocate
            return nil
          else
            # This not found. create it.
            in_scope = @block_scope.last
            idx = in_scope.size
            lcl = in_scope[name]
            lcl.created_in_block!(idx)
            if in_scope.from_eval
              depth = 1
            else
              depth = 0
            end
          end
        end

        lcl.access_in_block!

        return [lcl, depth]
      end

      def allocate_stack
        return nil if @use_eval
        # This is correct. the first one is 1, not 0.
        @alloca += 1
      end

      def allocate_slot
        i = @slot
        @slot += 1
        return i
      end

      def module_body?
        false
      end
    end

    # Example: m(1, *a)
    #            ^^^^^
    # The arguments sexp will be of type argscat
    #
    # [:call, :m, [:argscat, [:array, [:lit, 1]], [:call, nil, :a, [:arglist]]]]
    #
    class ConcatArgs < DynamicArguments
      kind :argscat

      def args(rest, array)
raise "no"
        @array = array

        if rest.kind_of? Array # TODO - When does this happen?
          @rest = rest
        else
          @rest = rest.body
        end
      end

      attr_accessor :array, :rest
    end

    class ConstAccess < Node
      kind :colon2

      def args(parent, name)
        @parent, @name = parent, name
      end

      attr_accessor :parent, :name

      def normalize(one, two=nil)
        if two
          args(one, two)
          node = self
        else
          node = ConstFind.new(@compiler)
          node.args(one)
        end

        return node
      end
    end

    class ConstAtTop < Node
      kind :colon3

      def args(name)
        @name = name
      end
      attr_accessor :name

      def parent # allows use like a colon2
        nil
      end
    end

    class ConstFind < Node
      kind :const

      def args(name)
        @name = name
      end

      attr_accessor :name
    end

    class ConstSet < Node
      kind :cdecl

      def args(name, val=nil)
        @from_top = false
        @parent   = nil
        @value    = val

        if Symbol === name then
          @name     = name
        elsif name.is? ConstAtTop then
          @from_top = true
          @name     = name.name
        else
          @parent   = name.parent
          @name     = name.name
        end
      end

      attr_accessor :from_top, :parent, :value, :name
    end

    class Dasgn < Node # TODO: remove
      kind :dasgn

      def self.create(compiler, sexp)
        LocalAssignment.create(compiler, [:lasgn, sexp[1], sexp[2]])
      end
    end

    class DasgnCurr < Node # TODO: remove
      kind :dasgn_curr

      def self.create(compiler, sexp)
        LocalAssignment.create(compiler, [:lasgn, sexp[1], sexp[2]])
      end
    end

    class Define < ClosedScope
      kind :defn

      def consume(sexp)
        name, args, body = sexp

        set(:iter => false, :in_ensure => false) do
          args = super([args]) # FIX: that array is dumb
          body = super([body]) # FIX: that array is dumb
        end

        return [name, args, body]
      end

      def args(name, args, body)
        @name, @arguments, @body = name,  args, expand(body)
      end

      attr_accessor :name, :body, :arguments
    end

    class DefineSingleton < Define
      kind :defs

      def consume(sexp)
        object = sexp.shift
        out = super(sexp)
        out.unshift convert(object)

        return out
      end

      def args(obj, name, body, args)
        @object = obj

        super(name, body, args)
      end

      attr_accessor :object
    end

    class Defined < Node
      kind :defined

      def consume(sexp)
        expr = sexp[0]
        if expr[0] == :call
          expr[1] = convert(expr[1])
        end

        sexp
      end

      def args(expr)
        @expression = expr
      end

      attr_accessor :expression
    end

    class Dvar < Node # TODO: remove
      kind :dvar

      def self.create(compiler, sexp)
        return LocalAccess.create(compiler, [:lvar, sexp.last])
      end
    end

    class DynamicArguments < Node
    end

    class DynamicExecuteString < DynamicString
      kind :dxstr
    end

    # DynamicOnceRegex is identical to DynamicRegex, with
    # the exception that the interpolation is only run once.
    # This is done using the +o+ flag to the literal. Please
    # see DynamicRegex for more detailed documentation.
    #
    # Sexp tag: +:dregx_once+
    #
    # Example:
    #
    #   /a#{b}c/o   # Note the +o+ option
    #
    class DynamicOnceRegex < DynamicRegex
      kind :dregx_once
    end

    # DynamicRegex is a dynamic regexp literal, i.e.
    # one with an interpolated component. These behave
    # the same as DynamicStrings (they actually use :str
    # and :evstr nodes also), please see above for a more
    # thorough explanation.
    #
    # Sexp tag: +:dregx+
    #
    # Example:
    #
    #   /a#{b}c/
    #
    class DynamicRegex < DynamicString
      kind :dregx

      def args(str, *body)
        @string = str
        @body = body
        if body.last.kind_of? Fixnum
          @options = body.pop
        else
          @options = 0
        end
      end
    end

    # DynamicString is a dynamic string literal; i.e.,
    # one with an interpolated component. There are a
    # few notable things: the parser will process any
    # interpolations which themselves contain a string
    # literal into a plain string literal instead of
    # a dynamic string. The latter will only be in
    # effect for variable interpolation etc.
    #
    # Each dynamic string consists of two things: string
    # literals for the nondynamic parts and :evstr nodes
    # for the parts that need to be evaluated. The :dstr
    # node itself contains the starting literal (if any),
    # but all subsequent ones appear as additional :str
    # nodes interspersed with :evstrs. The :evstr nodes
    # are any executable code, so they will eventually
    # be unwrapped and the sexp there translated to AST.
    #
    # Sexp tag: +:dstr+
    #
    # Example:
    #
    #   puts "Hi #{name}, howzit?"
    #        ^^^^^^^^^^^^^^^^^^^^^
    #
    # Sexp from example:
    #
    #   [:dstr, "Hi ",
    #    [:evstr, [:call, nil, :name, [:arglist]]],
    #    [:str, ", howzit?"]]
    #
    #
    class DynamicString < StringLiteral
      kind :dstr

      def args(str, *body)
        @string = str
        @body   = body
      end

      attr_accessor :body
    end

    class DynamicSymbol < DynamicString
      kind :dsym
    end

    class EmptyArray < Node
      kind :zarray
    end

    class Ensure < Node
      kind :ensure

      def consume(sexp)
        opts = {}
        set(:in_ensure, opts) do
          sexp[0] = convert(sexp[0])
        end

        # Propagate did_return up to an outer ensure
        if ens = get(:in_ensure)
          ens[:did_return] = opts[:did_return]
          outer = true
        else
          outer = false
        end

        [sexp[0], convert(sexp[1]), opts[:did_return], outer]
      end

      def args(body, ens, ret, outer)
        @body, @ensure = body, ens
        @did_return = ret
        @outer_ensure = outer

        if @in_block = get(:iter)
          @check_var, _ = get(:scope).find_local :@lre
        end

        # Handle a 'bug' in parsetree
        if @ensure == [:nil]
          @ensure = nil
        end
      end

      attr_accessor :body, :ensure, :did_return, :outer_ensure
    end

    # EvalExpression is a special node and does not appear in the Ruby
    # parse tree itself. It is inserted as the top-level scope when an
    # eval is run, which allows managing the specialized behaviour that
    # is needed for it.
    #
    class EvalExpression < Expression
      kind :eval_expression

      def consume(sexp)
        super(sexp, @in_iter, true)
      end

      def locals
        @my_scope
      end

      class DynamicLocal
        attr_reader :name

        def initialize(name)
          @name = name
        end

        def assigned!
        end
      end

      def initialize(comp)
        super(comp)

        unless comp.custom_scopes?
          raise ArgumentError, "only use with custom scopes"
        end

        @top_scope, @block_scope, @all_scopes, @context = comp.create_scopes
        @slot = @top_scope.size

        if @block_scope.empty?
          @my_scope = @top_scope
          @in_iter = false
        else
          @my_scope = @block_scope.last
          @in_iter = true
        end

        # Setup stuff so allocate_slot works
        @my_scope.scope = self
        @slot = @my_scope.size
      end

      def find_local(name, in_block=false, allocate=true)
        if normal = super(name, in_block, false)
          return normal
        end

        unless allocate
          if dynamic = @context.dynamic_locals
            return nil unless dynamic.key? name
          else
            return nil
          end
        end

        return DynamicLocal.new(name)
      end

      def enlarge_context
        return # HACK disable for now

#         locals = @context.locals

#         if !locals
#           if @my_scope.size > 0
#             @context.locals = Tuple.new(@my_scope.size)
#             @context.method.local_names = @my_scope.encoded_order
#           end
#         elsif @my_scope.size > locals.size
#           @context.locals = locals.enlarge(@my_scope.size)
#           @context.method.local_names = @my_scope.encoded_order
#         end
      end

    end

    class ExecuteString < StringLiteral
      kind :xstr
    end

    class Expression < Snippit
      kind :expression
    end

    # False is the literal +false+.
    #
    # Sexp tag: +:false+
    #
    # Example:
    #
    #   puts "Hi" if false
    #                ^^^^^
    #
    class False < Node
      kind :false
    end

    class File < Node
      kind :file
    end

    class FloatLiteral < NumberLiteral
      kind :float

      def args(value)
        @value = value
      end

      attr_accessor :value
    end

    class For < Iter
      kind :for

      # [[:newline, 1, "(eval)", [:dot2, [:lit, 1], [:lit, 2]]], [:lasgn, :x, 0]]
      # should become
      # [[:call, [:newline, 1, "(eval)", [:dot2, [:lit, 1], [:lit, 2]]], :each], [:lasgn, :x, 0] ]
      def self.create(compiler, sexp)
        # sexp[0] is :for
        # sexp[1] is the enumeration for each
        # sexp[2] is the arguments
        # sexp[3] is the body, if any
        sexp = s(sexp[0], s(:call, sexp[1], :each), sexp[2], sexp[3])
        super(compiler, sexp)
      end

      def consume(sexp)
        converted = convert(sexp[0])
        sexp[0] = converted # enum for the 'each' call

        # 'declare' the vars outside the for.
        scope = get(:scope)

        vars = sexp[1]

        block = get(:iter)

        if vars.first == :masgn
          ary = vars[1]
          1.upto(ary.size - 1) do |i|
            scope.find_local ary[i][1], block if ary[i].first == :lasgn
          end
        else
          scope.find_local vars[1], block if vars.first == :lasgn
        end

        set(:iter) do
          @locals = get(:scope).new_block_scope do
            set(:iter_args) do
             sexp[1] = convert(s(:iter_args, sexp[1])) # local var assignment
            end

            sexp[2] = convert(sexp[2]) # body
          end
        end

        sexp
      end

      def normalize(c, arguments, body)
        @arguments, @body = arguments, expand(body)

        c.block = self
        return c
      end
    end

    class GVar < Node
      kind :gvar

      def args(name)
        @name = name
      end

      attr_accessor :name
    end

    class GVarAssign < Node
      kind :gasgn

      def args(name, value=nil)
        @name, @value = name, value
      end

      attr_accessor :name, :value

      def optional
        []
      end

      def required
        [name]
      end
    end

    class HashLiteral < Node
      kind :hash

      def args(*body)
        @body = body
      end

      attr_accessor :body
    end

    class IVar < Node
      kind :ivar

      def normalize(name)
        @name = name
        return self
      end

      attr_accessor :name

    end

    class IVarAssign < Node
      kind :iasgn

      def normalize(name, val=nil)
        @value = val
        @name = name
        return self
      end

      def optional
        []
      end

      def required
        [name]
      end

      attr_accessor :name, :value
    end

    # If is the safe and familiar conditional as well as
    # the inverse: +unless+. The parser simply constructs
    # a negated version of the +if+ on those occasions.
    # The contents are the condition expression as well
    # as the expressions for then and else.
    #
    # Sexp tag: +:if+
    #
    # Example:
    #
    #   if true; puts "hi"; end
    #   ^^
    #
    class If < Node
      kind :if

      def args(cond, thn, els)
        @condition, @then, @else = cond, thn, els
      end

      attr_accessor :condition, :then, :else
    end

    class ImplicitHash < HashLiteral
      kind :ihash
    end

    class Iter < Node
      kind :iter

      def consume(sexp)
        c = convert(sexp[0])
        sexp[0] = c

        # Get rid of the linked list of dasgn_curr's at the top
        # of a block in at iter.
        if sexp.length > 2   # Fix for empty block
          first = sexp[2][1]
          if first.kind_of?(Array) and first[0] == :dasgn_curr
            if first[2].nil? or first[2][0] == :dasgn_curr
              sexp[2].delete_at(1)
            end
          end
        end

        if c.is? Call and c.method == :loop
          sexp[1] = convert(sexp[1])
          sexp[2] = convert(sexp[2])
          return sexp
        end

        set(:iter) do
          @locals = get(:scope).new_block_scope do
            set(:iter_args) do
              sexp[1] = convert(s(:iter_args, sexp[1]))
            end

            sexp[2] = convert(sexp[2])
          end
        end

        sexp
      end

      def normalize(c, a, b)
        @arguments, @body = a, expand(b)

        if c.is? Call and c.method == :loop
          n = Loop.new(@compiler)
          n.args(b)
          return n
        end

        c.block = self

        return c
      end

      attr_accessor :arguments, :body, :locals
    end

    class IterArgs < Node
      kind :iter_args

      def consume(sexp)
        args = sexp[0]

        if args.nil? then
          @arity = -1
        elsif args == 0 then
          @arity = 0
        else
          case args[0]
          when :masgn then
            node = MAsgn.new(@compiler)
            node.block_args = true
            args.shift

            # UR's masgn for "l { |*| }" is just s(:masgn, s(:splat)). So if we see this,
            # unshift nil to hold the place of the normal assigns. That puts
            # everything in the right position

            if args.size == 1
              args.unshift nil if args[0][0] == :splat
            end

            args[0] = convert(args[0]) if args[0]

            # Looks like UnifiedRuby throws a s(:splat) in if there is an anon
            # splat.

            if args[1]
              if args[1][0] == :splat
                args[1] = true
              else
                args[1] = convert(args[1])
              end
            end

            begin
              node.args(*args)
            rescue ArgumentError => e
              e2 = ArgumentError.new e.message +
                ": #{node.class} for IterArgs#args passed: #{args.inspect}"
              e2.set_backtrace e.backtrace
              raise e2
            end

            @arity = node.arity
            return [node]
          else
            @arity = 1
            return [convert(sexp[0])]
          end
        end

        return nil
      end

      def args(child)
        @child = child
      end

      def names
        return [] if @child.nil?

        if @child.is? LocalAssignment
          [@child.name]
        else
          @child.assigns.body.map { |i| i.name }
        end
      end

      attr_reader :arity

      def optional
        case @child
        when Fixnum, nil
          []
        else
          @child.optional
        end
      end

      def required
        return [] if @child.nil?
        @child.required
      end

      attr_accessor :child
    end

    # Literal is the default representation of any literal
    # object value in the code, such as a number representing
    # a Float. Fixnums and Regexps are delegated to be processed
    # by NumberLiteral and RegexLiteral respectively, but any
    # other is contained within as an object.
    #
    # The remaining literals will also have special treatment
    # in that they are stored in the Literals Tuple of the
    # CompiledMethod so that they are accessible to runtime
    # code.
    #
    # Sexp tag: +:lit+
    #
    # Example:
    #
    #   pi_ish = 3.14
    #            ^^^^
    #
    #   drive  = :dvd_rom
    #            ^^^^^^^^
    #
    class Literal < Node
      kind :lit

      def normalize(value)
        @value = value

        case value
        when Fixnum
          nd = NumberLiteral.new(@compiler)
          nd.args(value)
          return nd
        when Regexp
          nd = RegexLiteral.new(@compiler)
          nd.args(value.source, value.options)
          return nd
        when ::Range
          if value.exclude_end?
            nd = RangeExclude.new(@compiler)
          else
            nd = Range.new(@compiler)
          end

          start = NumberLiteral.new(@compiler)
          start.args value.begin

          fin = NumberLiteral.new(@compiler)
          fin.args value.end

          nd.args start, fin
          return nd
        end

        return self
      end

      attr_accessor :value
    end

    class LocalAccess < LocalVariable
      kind :lvar

      def args(name, idx = nil)
        @name = name
        super(name)
      end

      attr_accessor :name
    end

    class LocalAssignment < LocalVariable
      kind :lasgn

      def args(name, val=nil)
        # val will be nil if this is e.g. an lasgn inside an masgn
        @value = val
        super(name)

        if ArrayLiteral === @value then
          @splat   = @value.body.grep(Splat).first
          @value.body.reject! { |o| Splat === o } if @splat
        else
          @splat = nil
        end

        @variable.assigned!
      end

      attr_accessor :name, :variable, :value

      def from_variable(var, value=nil)
raise "no"

        super(var)

        @value = value
      end

      def optional
        []
      end

      def required
        [name]
      end
    end

    class LocalVariable < Node
      def consume(sexp)
        name = sexp[0]

        scope = get(:scope)

        if get(:iter)
          @variable, @depth = scope.find_local name, true
        else
          @variable, @depth = scope.find_local name
        end

        super(sexp)
      end

      def args(name)
        @name = name
      end

      def from_variable(var, depth=nil)
        @variable = var
        @depth = depth
        @name = var
      end
    end

    class Loop < Node
      def args(body)
        @body = body
      end
    end

    class MAsgn < Node
      kind :masgn
      attr_accessor :block_args

      def initialize(comp)
        super(comp)
        @block_args = false
      end

      def args(lhs, rhs = nil)
        # TODO: rename the ivars as well... nonsensical names...
        @lhs = lhs
        @rhs = rhs

        # FIX: this extraction should NOT be done
        @splat_lhs = lhs.body.grep(Splat).first
        @lhs.body.reject! { |o| Splat === o } if @splat_lhs

        # FIX: this extraction should NOT be done
        if ArrayLiteral === rhs then
          @splat_rhs = rhs.body.grep(Splat).first
          @rhs.body.reject! { |o| Splat === o } if @splat_rhs
        else
          @splat_rhs = nil
        end

        # TODO: fix in sexp
        unless ArrayLiteral === @lhs then
raise "huh"
          @lhs, @splat_lhs = nil, @lhs
        end

        @in_block = false
      end

      def assigns
        @lhs
      end

      attr_accessor :splat, :source, :in_block

      def empty?
        @lhs.nil? and (@splat_lhs.equal?(true) or @splat_lhs.nil?)
      end

      def optional
        []
      end

      def required
        return [] if @lhs.nil?
        @lhs.body.map { |i| i.kind_of?(MAsgn) ? i.required : i.name }.flatten
      end

      def arity
        if @lhs
          fixed = @lhs.body.size
        else
          fixed = 0
        end

        if @splat_lhs
          fixed += 1
          return -fixed
        end

        return fixed
      end
    end

    # Implicit regexp matching node. A Match is created if
    # there is a regexp literal in a condition without an
    # object to match against (or indeed the matching op.)
    # Ruby allows this form to match against +$_+ which is
    # a predefined global always set to the last line of
    # input read into the program.
    #
    # Sexp tag: +:match+
    #
    # Example:
    #
    #   gets
    #   puts "Uh-uh, you said 'foo'" if /foo/
    #                                  ^^^^^^^^
    #
    class Match < Node
      kind :match

      def args(pattern)
        @pattern = pattern
      end

      attr_accessor :pattern
    end

    # Match2 is a regexp match where the regexp literal is on the
    # left hand side of the match operator. This node is generated
    # any time such an event occurs. Naturally, the parser is not
    # able to determine whether a variable is a Regexp, so it only
    # works with a regexp literal. See also Match3.
    #
    # Sexp tag: +:match2+
    #
    # Example:
    #
    #   /this/ =~ "matches this"
    #   ^^^^^^^^^
    #
    class Match2 < Node
      kind :match2

      def args(pattern, target)
        @pattern, @target = pattern, target
      end

      attr_accessor :pattern, :target
    end

    # Match3 is a regexp match where the regexp literal is on the
    # right hand side of the match operator. This node is generated
    # any time such an event occurs. Naturally, the parser is not
    # able to determine whether a variable is a Regexp, so it only
    # works with a regexp literal. See also Match2.
    #
    # Sexp tag: +:match3+
    #
    # Example:
    #
    #   "this matches" =~ /this/
    #                  ^^^^^^^^^
    #
    class Match3 < Node
      kind :match3

      def args(pattern, target)
        @pattern, @target = pattern, target
      end

      attr_accessor :target, :pattern
    end

    class MethodCall < Node # TODO: rename to Call and fold together
      def initialize(comp)
        super(comp)
        @block = nil
        scope = get(:scope)
        if scope.is? Class
          @scope = :class
        elsif scope.is? Module
          @scope = :module
        elsif scope.is? Script
          @scope = :script
        else
          @scope = :method
        end
      end

      def consume(sexp)
        @line = sexp.line
        @file = sexp.file

        super
      end

      def block=(obj)
        if obj.kind_of? Iter
          @check_var, _ = get(:scope).find_local :@lre
        end

        @block = obj
      end

      attr_reader :block, :check_var
      attr_accessor :scope
    end

    class Module < ClosedScope
      kind :module

      def args(name, parent, body)
        @name, @parent, @body = name, parent, body
      end

      def consume(sexp)
        if sexp[0].kind_of? Symbol
          sym = sexp[0]
          parent = nil
        else
          # name is not just the name as a symbol, it's something else...
          # Turn the sexp into a Node object
          name_node = convert(sexp[0])
          sym = name_node.name

          if name_node.is? ConstFind
            parent = nil
          elsif name_node.is? ConstAtTop
            parent = name
          else
            parent = name_node.parent
          end
        end

        body = set(:namespace, sym) do
          super([sexp.last])
        end

        [sym, parent, body]
      end

      attr_accessor :name, :body, :parent

      def module_body?
        true
      end
    end

    # Negate represents a negative numeric literal.
    # It contains the Ruby object for the absolute
    # value, the node itself is used as the negative
    # marker.
    #
    # Sexp tag: +:negate+
    #
    # Example:
    #
    #   -1 + -0.045
    #   ^^   ^^^^^^
    #
    class Negate < Node
      kind :negate

      def args(child)
        @child = child
      end

      attr_accessor :child
    end

    # Newline handles :newline nodes, which are inserted by the parser to
    # allow keeping track of the file and line a certain sexp was produced
    # from. In addition to that metadata, it contains within it a Block of
    # the actual Ruby code (as sexp) that makes up that particular line.
    #
    # Sexp tag: +:newline+
    #
    class Newline < Node
      kind :newline

      def consume(sexp)
        @compiler.set_position sexp[1], sexp[0]
        super sexp
      end

      def args(line, file, child=nil)
        @line, @file, @child = line, file, child
      end

      attr_accessor :line, :file, :child

      def is?(cls)
        @child.kind_of? cls
      end
    end

    class Next < Break
      kind :next
    end

    # Nil is the literal +nil+.
    #
    # Sexp tag: +:nil+
    #
    # Example:
    #
    #   puts "Hi" if nil
    #                ^^^
    #
    class Nil < Node
      kind :nil
    end

    # Not is either +not+ or +!+. The precedence
    # has been resolved by the parser, the two
    # types may have different parents and children
    # because of it, though.
    #
    # It contains the expression to negate.
    #
    # Sexp tag: +:not+
    #
    # Example:
    #
    #   not available?
    #   ^^^
    #
    #   !true
    #   ^
    #
    class Not < Node
      kind :not

      def args(child)
        @child = child
      end

      attr_accessor :child
    end

    # NthRef is one of the numbered groups from the last regexp match.
    # The node contains the numeric value, which will then be combined
    # with $ to make the global at runtime. Technically there is no
    # limitation (up to the thousands) on the number of backrefs but
    # Win32 does limit it to 10. See also BackRef for the other regexp
    # match automatic (thread-) globals.
    #
    # Sexp tag: +:nth_ref+
    #
    # Example:
    #
    #     /(f)oo/ =~ variable
    #
    #     puts $1
    #          ^^
    #
    class NthRef < Node
      kind :nth_ref

      def args(which)
        @which = which
      end

      attr_accessor :which
    end

    # NumberLiteral is generated from a Literal node that
    # represents a Fixnum literal as a convenience. It
    # contains the actual number in a Ruby Fixnum. These
    # are positive numbers only; a combination of Negate
    # and Literal is used for negatives.
    #
    # Sexp tag: N/A
    #
    # Example:
    #
    #   a = 50
    #       ^^
    #
    class NumberLiteral < Node
      kind :fixnum

      def args(value)
        @value = value
      end

      attr_accessor :value
    end

    # h[:a] &&= 3 (special case)
    # h[:a] ||= 3 (special case)
    # h[:a] +=  3
    # h[:a] -=  3
    # ....
    # all binary message plus = variants
    class OpAssign1 < Node
      kind :op_asgn1

      def consume(sexp)
        # Detect PT form
        if sexp.size == 4
          idx = convert(sexp[1]).body
          which = sexp[2]

          case which
          when :"||"
            which = :or
          when :"&&"
            which = :and
          end

          val = convert(sexp[3])
        else
          # Value to be op-assigned is always first element of value
          sexp[2].shift # Discard :array token
          val = convert(sexp[2].shift)
          # Remaining elements in value are index args excluding final nil marker
          idx = []
          while sexp[2].size > 1 do
            idx << convert(sexp[2].shift)
          end
          which = sexp[1]
        end
        [convert(sexp[0]), which, idx, val]
      end

      def args(obj, kind, index, value)
        @object, @kind, @index, @value = obj, kind, index, value
      end

      attr_accessor :object, :kind, :value, :index
    end

    # h.a ||= 3 (special case)
    # h.a &&= 3 (special case)
    # h.a +=  3
    # h.a -=  3
    # ....
    # all binary message plus = variants
    class OpAssign2 < Node
      kind :op_asgn2

      def args(obj, method, kind, assign, value=nil)
        @object = obj

        # Detect PT form
        unless value then
          @method = method.to_s.sub(/=$/, '').to_sym
          @assign = method
          @value = assign
          @kind = kind

          case kind
          when :"||"
            @kind = :or
          when :"&&"
            @kind = :and
          end
        else
          @object, @method, @kind, @value = obj, method, kind, value
          str = assign.to_s
          if str[-1] == ?=
            @assign = assign
          else
            str << "="
            @assign = str.to_sym
          end
        end
      end

      attr_accessor :object, :method, :kind, :assign, :value
    end

    class OpAssignAnd < Node
      kind :op_asgn_and

      def args(left, right)
        @left, @right = left, right
      end

      attr_accessor :left, :right
    end

    class OpAssignOr < OpAssignAnd
      kind :op_asgn_or
    end

    # Or represents either +or+ or +||+.
    # The precedence difference between the
    # two has been resolved by the parser so
    # both types map into this node although
    # their contexts (parents and children)
    # may be different.
    #
    # It contains both the left and the right
    # subexpression.
    #
    # Sexp tag: +:or+
    #
    # Example:
    #
    #   foo or bar
    #       ^^
    #
    #   baz || quux
    #       ^^
    #
    class Or < And
      kind :or
    end

    class PostExe < Call
      kind :postexe

      def normalize(recv__ignored, meth, args=nil)
        @method, @arguments = meth, args

        collapse_args()
        return self
      end

      attr_accessor :method, :arguments

      def call?
        false
      end

      # Treat a :postexe node as if it were a call to at_exit
      def self.create(compiler, sexp)
        sexp = s(:call, nil, :at_exit, s(:arglist))
        super(compiler, sexp)
      end
    end

    # Example: h[*a] = 1
    #            ^^    ^
    # The arguments sexp of the call will be of type argspush.
    # The RHS is always a single item, even if there are commas
    # on the RHS of the =.
    #
    # The value inside [] will by of type :splat, with the real
    # sexp as it's sole argument.
    #
    # For: h[*a] = 1
    #
    # [:attrasgn, [:call, nil, :h, [:arglist]], :[]=,
    #   [:argspush,
    #     [:splat, [:call, nil, :a, [:arglist]]]],
    #     [:lit, 1]]]
    #
    # For: h[*a] = 1, 2
    #
    # [:attrasgn, [:call, nil, :h, [:arglist]], :[]=,
    #  [:argspush,
    #    [:splat, [:call, nil, :a, [:arglist]]]],
    #    [:svalue, [:array, [:lit, 1], [:lit, 2]]]]]
    #
    # In this case, given a = [:blah], the []= method will be given 2 arguments:
    # :blah and [1,2]
    #
    class PushArgs < DynamicArguments
      kind :argspush

      def args(array, item)
raise "no"
        @item = item
        unless array.is? Splat
          raise Error, "Unknown form of argspush: #{array.class}"
        end

        @array = array.child
      end

      attr_accessor :array, :item
    end

    class Range < Node
      kind :dot2

      def args(start, fin)
        @start, @finish = start, fin
      end

      attr_accessor :start, :finish
    end

    class RangeExclude < Range
      kind :dot3
    end

    class Redo < Break
      kind :redo
    end

    # RegexLiteral is a regular expression literal.
    # It is usually generated directly but may also
    # be delegated to by Literal. Each RegexLiteral
    # contains the source (which is actually a String)
    # and a Fixnum representing the regexp options in
    # effect. These two bits of information are used
    # to create the actual object through Regexp.new
    # at runtime.
    #
    # Sexp tag: +:regex+   (Note missing "p.")
    #
    # Example:
    #
    #   puts "matched" if /foo/ =~ variable
    #                     ^^^^^
    #
    class RegexLiteral < Node
      kind :regex

      def args(source, options)
        @source, @options = source, options
      end

      attr_accessor :source, :options
    end

    class Rescue < Node
      kind :rescue

      def args(body, res, els)
        @body, @rescues, @else = body, res, els
      end

      def consume(sexp)
        body = sexp.shift if sexp.first && sexp.first.first != :resbody
        els  = sexp.pop   if sexp.last  && sexp.last.first  != :resbody
        res  = sexp

        body = convert(body) if body
        set(:in_rescue) do
          res.map! { |r| convert(r) }
          res = nil if res.empty?
        end
        els = convert(els) if els

        [body, res, els]
      end

      attr_accessor :body, :rescue, :else
    end

    class RescueCondition < Node
      kind :resbody

      def args(cond, body = nil)
        @body = body
        @splat = nil

        # eg rescue => e (no exception class listed)
        if cond.is?(ArrayLiteral) &&
            (cond.body.empty? ||
             (!cond.body.first.kind_of?(ConstFind) &&
              !cond.body.first.kind_of?(ConstAccess))) then
          cf = ConstFind.new(@compiler)
          cf.args :StandardError
          cond.body.unshift cf
        end


        if cond.is? ArrayLiteral
          @splat = cond.body.grep(Splat).first
          cond.body.reject! { |o| Splat === o } if @splat
          @conditions = cond.body
        elsif cond.is? Splat
          @conditions = nil
          @splat = cond.child
        else
          raise Error, "Unknown rescue condition form"
        end

        if body.nil?
          @body = Nil.new(@compiler)
        end
      end

      attr_accessor :conditions, :splat, :body, :next
    end

    class Retry < Break
      kind :retry
    end

    class Return < Node
      kind :return

      def args(val=nil)
        @splat = false

        if val && val.is?(ArrayLiteral)
          @splat = val.body.grep(Splat).first
        end

        @value = val

        @in_rescue = get(:in_rescue)

        if ens = get(:in_ensure)
          ens[:did_return] = true
          @in_ensure = true
        else
          @in_ensure = false
        end

        if @in_block = get(:iter)
          @check_var, _ = get(:scope).find_local :@lre
        end
      end

      attr_accessor :value, :in_rescue, :in_ensure, :in_block, :check_var, :splat
    end

    class SClass < ClosedScope
      kind :sclass

      def args(obj, body)
        @object, @body = obj, body
      end

      def consume(sexp)
        [convert(sexp[0]), super([sexp[1]])]
      end

      attr_accessor :object, :body
    end

    # Wrapper around a multi-element expression
    # Examples include: x = 1,2,3; return *1; x = *r; x = 1,2,3,*y
    class SValue < Node
      kind :svalue

      # If a Splat is contained in this SValue, it will be
      # stored in '@splat'
      # If there is an ArrayLiteral, it will be stored in '@literal'
      def args(contents)
        case contents
        when ArrayLiteral then
          @splat = contents.body.grep(Splat).first
          contents.body.reject! {|e| Splat === e} if @splat
          @literal = contents
        when Splat then
          @splat = contents
          @literal = nil
        else
          @splat = nil
          @literal = contents
        end
      end

      attr_reader :splat, :literal
    end

    # +Scope+ is another special node type. It represents the scope
    # inside a logical Ruby unit such as a method definition or a
    # class definition. Curiously, though, the Scope is not where
    # the variable scoping etc. happens: for that, see ClosedScope
    # and its subclasses. For example in a method definition, the
    # Define node itself is the ClosedScope, but it in turn has a
    # Scope object encapsulated inside. To make things stranger,
    # the Scope object itself contains the Block of code (method
    # body, for example) but _also the names of local variables_
    # inside that code block. The locals are *actually* managed
    # by the ClosedScope, though. So for the purposes of Rubinius
    # compilation, you can sort of ignore the Scope node except
    # for its semantic meaning.
    #
    # Sexp tag: +:scope+
    #
    # Example:
    #
    #   def foo(a, b)   <
    #     ...           <  Scope object created inside the Define
    #   end             <
    #
    class Scope < Node
      kind :scope

      def consume(sexp)
        if sexp[0].nil?
          return [nil, nil]
        end

        # Handle def self.foo; end, which unlike def foo; end does not generate a block
        if sexp[0].first == :args
          sexp[0] = [:block, sexp[0], [:nil]]
        end

        sexp[0] = convert(sexp[0])
        # Fake the locals
        if sexp.size == 1
          sexp << []
        end

        return sexp
      end

      def args(block, locals)
        @block, @locals = block, locals
      end

      attr_accessor :block, :locals

      def empty?
        @block.nil?
      end
    end

    # Script is a special node, and does not exist in normal Ruby code.
    # It represents the top-level of a .rb file, and as such is the most
    # common top-level container.
    #
    class Script < ClosedScope
      kind :script

      def args(body)
        @body = expand(body)
      end

      attr_accessor :body

      def name
        :__script__
      end
    end

    # Self is the literal +self+.
    #
    # Sexp tag: +:self+
    #
    # Example:
    #
    #   p self
    #     ^^^^
    #
    class Self < Node
      kind :self
    end

    class SetSlot < Node
      def args(idx, val)
        @index, @value = idx, val
      end

      attr_reader :index, :value
    end

    # Snippit was a special type of a scope, not exactly a Script but
    # not something else either. Currently it only provides abstract
    # support for eval, for example.
    #
    class Snippit < ClosedScope
      kind :snippit

      def consume(sexp, iter=false, eval=false)
        set(:family, self) do
          super(sexp, iter, eval)
        end
      end

      def args(body)
        @body = expand(body)
      end

      attr_accessor :body
    end

    # Example: m(*a)
    #            ^^
    # The arguments sexp will be of type splat
    #
    # [:call, nil, :m, [:splat, [:call, nil, :a, [:arglist]]]]
    #
    class Splat < DynamicArguments
      kind :splat

      def args(child = nil)
        @child = child
      end

      attr_accessor :child

      def inspect
        name = self.class.name.split(/::/).last
        "#{name}[#{@child.inspect}]"
      end
    end

    # StringLiteral is a nondynamic string literal.
    # It contains the Ruby String object corresponding
    # to the real given character sequence. Since these
    # objects are stored in the Literals Tuple, you will
    # often see bytecode that performs a +string_dup+,
    # which just makes a copy of the stored one so that
    # the user can modify his version.
    #
    # Sexp tag: +:str+
    #
    # Example:
    #
    #   puts "hi"
    #        ^^^^
    #
    class StringLiteral < Node
      kind :str

      def args(str)
        @string = str
      end

      attr_accessor :string
    end

    class Super < Call
      kind :super

      def args(*args)
        @method = get(:scope)
        @arguments = ArgList.new(@compiler)
        @arguments.body = args

        collapse_args()
      end

      attr_accessor :arguments
    end

    class ToArray < Node
      kind :to_ary

      def args(child)
        @child = child
      end

      attr_accessor :child
    end

    # "#{foo.bar}"
    #
    # Evstrs appear as a part of a dstr, dynamic string.
    # The evstr part is the code to be evaluated while
    # the dstr puts the whole thing together.
    #
    # Interestingly, an evstr that only contains string
    # literals such as "#{'foo'}" is parsed to a plain
    # string literal. This is the same for dregx.
    class ToString < Node
      kind :evstr

      # Expected input is a sub-sexp that represents the
      # code to be run when evaluating or empty sexp.
      def consume(sexp)
        sexp = [s(:str, "")] if sexp.empty?
        super(sexp)
      end

      def args(child)
        @child = child
      end

      attr_accessor :child
    end

    # True is the literal +true+.
    #
    # Sexp tag: +:true+
    #
    # Example:
    #
    #   puts "Hi" if true
    #                ^^^^
    #
    class True < Node
      kind :true
    end

    # +Undef+ is the keyword +undef+, #undefine_method does not have any
    # special handling that would make it processable here and thus is
    # just a normal method call. Two data are retained: first naturally
    # the name of the method in question which can be either just the name
    # as a plain identifier or given as a Symbol. The other information
    # is whether the undef occurs inside a Module definition scope. The
    # undef keyword is somewhat surprising in its behaviour: it always
    # tries to undefine the given method in the module that the line is
    # _lexically_ enclosed in, so this must be known for the cases it is
    # used outside the Module definition scopes. This also excludes any
    # use in the block form of Module.new, which may be surprising altough
    # consistent with closure behaviour.
    #
    # Sexp tag: +:undef+
    #
    # Example:
    #
    #   class Foo
    #     undef bar
    #     ^^^^^^^^^
    #   end
    #
    class Undef < Node
      kind :undef

      def args(name)
        raise "name should be lit: #{name.inspect}" unless Literal === name
        @name = name.value
        scope = get(:scope)
        if scope.is? Node::Class or scope.is? Node::Module
          @in_module = true
        else
          @in_module = false
        end
      end

      attr_accessor :name, :in_module
    end

    # Until is same as a +while not+. See While for further
    # documentation. May also be used pre- or postcondition.
    #
    # Sexp tag: +:until+
    #
    # Example:
    #
    #   # Condition first (optionally add +do+ after the condition.)
    #   until blah
    #   ^^^^^^^^^^
    #     ...
    #   end
    #
    #   # Run once first. +end+ and +until+ must be on the same line
    #   begin
    #     ...
    #   end until blah
    #   ^^^^^^^^^^^^^^
    #
    class Until < While
      kind :until
    end

    class VAlias < Node
      kind :valias

      def args(current, name)
        @current, @new = current, name
      end

      attr_accessor :current, :new
    end

    class When < Node
      kind :when

      def args(cond, body = nil)
        @body = expand(body)
        @conditions = []
        @splat = nil

        if cond.is? ArrayLiteral
          cond.body.each do |c|
            # Inner when means splat.
            if c.is? Node::When
              if c.splat
                @splat = c.splat
              else
                @splat = c.conditions
              end
            else
              @conditions << c
            end
          end
        else
          @splat = cond
        end
      end

      attr_reader :body, :conditions, :splat
    end

    # While is the standard conditional looping construct.
    # It contains the condition itself as well as the body
    # to run; in addition, it may also indicate that the
    # condition should be checked before or after the first
    # loop, default of course being before. The syntax for
    # both is below.
    #
    # Sexp tag: +:while+
    #
    # Example:
    #
    #   # Condition first (optionally add +do+ after the condition.)
    #   while blah
    #   ^^^^^^^^^^
    #     ...
    #   end
    #
    #   # Run once first. +end+ and +while+ must be on the same line
    #   begin
    #     ...
    #   end while blah
    #   ^^^^^^^^^^^^^^
    #
    class While < Node
      kind :while

      def args(cond, body, check_first=true)
        @condition, @body, @check_first = cond, expand(body), check_first
      end

      attr_accessor :condition, :body, :check_first
    end

    class Yield < Call
      kind :yield

      def args(*args)
        @arguments = args
      end

      attr_accessor :arguments
    end

    class ZSuper < Super
      kind :zsuper

      def args
        @method = get(:scope)
        if @method.is? SClass
          @method = @method.outer_scope
        end
      end
    end
  end
end
