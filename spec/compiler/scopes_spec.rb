require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "populates argument information" do
    sexp = s(:args, :mand1, :mand2, :opt1, :opt2, :"*rest", :"&block",
             s(:block,
               s(:lasgn, :opt1, s(:fixnum, 42)),
               s(:lasgn, :opt2, s(:fixnum, 24))))

    @comp = Compiler.new TestGenerator
    scope = Compiler::Node::ClosedScope.new @comp
    args = @comp.set :scope, scope do
      @comp.convert_sexp sexp
    end

    args.required.map { |m| m.name }.should == [:mand1, :mand2]
    args.optional.map { |m| m.name }.should == [:opt1, :opt2]
    args.splat.name.should == :rest
    args.block_arg.name.should == :block

    expected = [[:opt1, 42], [:opt2, 24]]
    args.defaults.map { |m| [m.name, m.value.value] }.should == expected
  end

  it "handles all kinds of arguments in a def" do
    ruby = "def f(mand1, mand2, opt1 = 42, opt2 = 24, *rest, &block) x(mand1, mand2, opt1, opt2, rest, block) end"

    sexp = s(:defn, :f,
             s(:args, :mand1, :mand2, :opt1, :opt2, :"*rest", :"&block",
               s(:block,
                 s(:lasgn, :opt1, s(:fixnum, 42)),
                 s(:lasgn, :opt2, s(:fixnum, 24)))),
             s(:scope,
               s(:block,
                 s(:call, nil, :x,
                   s(:arglist,
                     s(:lvar, :mand1),
                     s(:lvar, :mand2),
                     s(:lvar, :opt1),
                     s(:lvar, :opt2),
                     s(:lvar, :rest),
                     s(:lvar, :block))))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        opt_arg1 = d.new_label
        d.passed_arg 2
        d.git opt_arg1
        d.push 42
        d.set_local 2
        d.pop
        opt_arg1.set!

        opt_arg2 = d.new_label
        d.passed_arg 3
        d.git opt_arg2
        d.push 24
        d.set_local 3
        d.pop
        opt_arg2.set!

        d.push_block
        d.dup
        d.is_nil
        after = d.new_label
        d.git after
        d.push_const :Proc
        d.swap
        d.send :__from_block__, 1
        after.set!
        d.set_local 5
        d.pop

        d.push :self
        d.push_local 0
        d.push_local 1
        d.push_local 2
        d.push_local 3
        d.push_local 4
        d.push_local 5
        d.send :x, 6, true

        d.ret
      end

      g.push_context
      g.push_literal :f
      g.push_literal meth
      g.send :__add_method__, 2
    end

    defn = @node.body
    args = defn.arguments

    args.required.map { |m| m.name }.should == [:mand1, :mand2]
    args.optional.map { |m| m.name }.should == [:opt1, :opt2]
    args.splat.name.should == :rest
    args.block_arg.name.should == :block

    expected = [[:opt1, 42], [:opt2, 24]]
    args.defaults.map { |m| [m.name, m.value.value] }.should == expected
  end

  it "compiles a defn with no args" do
    ruby = <<-EOC
      def a
        12
      end
    EOC

    sexp = s(:defn, :a,
             s(:args),
             s(:scope,
               s(:block,
                 s(:fixnum, 12))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push 12
        d.ret
      end

      g.push_context
      g.push_literal :a
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles 'def add(a,b); a + b; end'" do
    ruby = <<-EOC
      def add(a, b); a + b; end
    EOC

    sexp = s(:defn, :add,
             s(:args, :a, :b),
             s(:scope,
               s(:block,
                 s(:call, s(:lvar, :a), :+, s(:arglist, s(:lvar, :b))))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push_local 0
        d.push_local 1
        d.meta_send_op_plus
        d.ret
      end

      g.push_context
      g.push_literal :add
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles 'def add(a,b=2); a + b; end'" do
    ruby = <<-EOC
      def add(a, b=2); a + b; end
    EOC

    sexp = s(:defn, :add,
             s(:args, :a, :b, s(:block, s(:lasgn, :b, s(:fixnum, 2)))),
             s(:scope,
               s(:block,
                 s(:call, s(:lvar, :a), :+, s(:arglist, s(:lvar, :b))))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        up = d.new_label
        d.passed_arg 1
        d.git up
        d.push 2
        d.set_local 1
        d.pop

        up.set!

        d.push_local 0
        d.push_local 1
        d.meta_send_op_plus
        d.ret
      end

      g.push_context
      g.push_literal :add
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles 'def add(a); [].each { |b| a + b }; end'" do
    ruby = <<-EOC
      def add(a); [].each { |b| a + b }; end
    EOC

    sexp = s(:defn, :add,
             s(:args, :a),
             s(:scope,
               s(:block,
                 s(:iter,
                   s(:call, s(:array), :each, s(:arglist)),
                   s(:lasgn, :b),
                   s(:call, s(:lvar, :a), :+, s(:arglist, s(:lvar, :b)))))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        iter = description do |i|
          i.cast_for_single_block_arg
          i.set_local_depth 0, 0
          i.pop
          i.push_modifiers
          i.new_label.set! # redo

          i.push_local 0                # a
          i.push_local_depth 0, 0       # b
          i.meta_send_op_plus
          i.pop_modifiers
          i.ret
        end

        d.passed_block(1) do
          d.make_array 0
          d.create_block iter
          d.send_with_block :each, 0, false
        end
        d.ret
      end

      g.push_context
      g.push_literal :add
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles 'def a(*b); nil; end' with no max argument count" do
    ruby = <<-EOC
      def a(*b); nil; end
    EOC

    sexp = s(:defn, :a,
             s(:args, :"*b"),
             s(:scope, s(:block, s(:nil))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push :nil
        d.ret
      end

      g.push_context
      g.push_literal :a
      g.push_literal meth
      g.send :__add_method__, 2
    end

    @splat.name.should == :b
  end

  it "compiles 'def a(&b); b; end'" do
    ruby = <<-EOC
      def a(&b); b; end
    EOC

    sexp = s(:defn, :a,
             s(:args, :"&b"),
             s(:scope,
               s(:block,
                 s(:lvar, :b))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push_block
        d.dup
        d.is_nil

        after = d.new_label
        d.git after

        d.push_const :Proc
        d.swap
        d.send :__from_block__, 1

        after.set!
        d.set_local 0
        d.pop
        d.push_local 0
        d.ret
      end

      g.push_context
      g.push_literal :a
      g.push_literal meth
      g.send :__add_method__, 2
    end
  end

  it "compiles 'def a.go; 12; end'" do
    ruby = <<-EOC
      def a.go; 12; end
    EOC

    sexp = s(:defs, s(:call, nil, :a, s(:arglist)), :go,
             s(:args),
             s(:scope,
               s(:block,
                 s(:fixnum, 12))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push 12
        d.ret
      end

      g.push :self
      g.send :a, 0, true
      g.send :metaclass, 0
      g.push_literal :go
      g.push_literal meth
      g.send :attach_method, 2
    end
  end

  it "compiles 'lambda { def a(x); x; end }'" do
    ruby = <<-EOC
      lambda { def a(x); x; end }
    EOC

    sexp = s(:iter, s(:call, nil, :lambda, s(:arglist)), nil,
             s(:defn, :a,
               s(:args, :x),
               s(:scope,
                 s(:block,
                   s(:lvar, :x)))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      lam = description do |l|
        meth = description do |m|
          m.push_local 0
          m.ret
        end

        l.pop
        l.push_modifiers
        l.new_label.set!
        l.push_context
        l.push_literal :a
        l.push_literal meth
        l.send :__add_method__, 2
        l.pop_modifiers
        l.ret
      end

      g.passed_block do
        g.push :self
        g.create_block lam
        g.send_with_block :lambda, 0, true
      end
    end
  end

  it "compiles 'class << x; 12; end'" do
    ruby = <<-EOC
      class << x; 12; end
    EOC

    sexp = s(:sclass, s(:call, nil, :x, s(:arglist)), s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      meth = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :__verify_metaclass__, 0
      g.pop
      g.open_metaclass
      g.dup
      g.push_literal meth
      g.swap
      g.attach_method :__metaclass_init__
      g.pop
      g.send :__metaclass_init__, 0
    end
  end

  it "compiles a class with no superclass" do
    ruby = <<-EOC
      class A; 12; end
    EOC

    sexp = s(:class, :A, nil, s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.push :nil
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0
    end
  end

  it "compiles a class declared at a path" do
    ruby = <<-EOC
      class A::B; 12; end
    EOC

    sexp = s(:class, s(:colon2, s(:const, :A), :B), nil,
             s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.push_const :A
      g.push :nil
      g.open_class_under :B
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0
    end
  end

  it "compiles a class with superclass" do
    ruby = <<-EOC
      class A < B; 12; end
    EOC

    sexp = s(:class, :A, s(:const, :B), s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.push_const :B
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0
    end
  end

  it "compiles a class with space allocated for locals" do
    ruby = <<-EOC
      class A; a = 1; end
    EOC

    sexp = s(:class, :A, nil,
             s(:scope, s(:lasgn, :a, s(:fixnum, 1))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 1
        d.set_local 0
        d.ret
      end

      g.push :nil
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0

    end
  end

  it "compiles a normal module" do
    ruby = <<-EOC
      module A; 12; end
    EOC

    sexp = s(:module, :A, s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.open_module :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__module_init__
      g.pop
      g.send :__module_init__, 0
    end
  end

  it "compiles a module declared at a path" do
    ruby = <<-EOC
      module A::B; 12; end
    EOC

    sexp = s(:module, s(:colon2, s(:const, :A), :B),
             s(:scope, s(:fixnum, 12)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      desc = description do |d|
        d.push_self
        d.add_scope
        d.push 12
        d.ret
      end

      g.push_const :A
      g.open_module_under :B
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__module_init__
      g.pop
      g.send :__module_init__, 0
    end
  end
end
