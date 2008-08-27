require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles 'blah'" do
    ruby = <<-EOC
      blah
    EOC

    sexp = s(:vcall, :blah)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :blah, 0, true
    end
  end

  it "compiles 'blah()'" do
    ruby = <<-EOC
      blah()
    EOC

    sexp = s(:fcall, :blah)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :blah, 0, true
    end
  end

  it "compiles 'blah(1, 2)'" do
    ruby = <<-EOC
      blah(1, 2)
    EOC

    sexp = s(:fcall, :blah, s(:array, s(:fixnum, 1), s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.push 1
      g.push 2
      g.send :blah, 2, true
    end
  end

  it "compiles 'blah(*a)'" do
    ruby = <<-EOC
      blah(*a)
    EOC

    sexp = s(:fcall, :blah, s(:splat, s(:vcall, :a)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil # TODO: what is the nil for?
      g.send_with_splat :blah, 0, true, false
    end
  end

  it "compiles 'block_given?'" do
    ruby = <<-EOC
      block_given?
    EOC

    sexp = s(:fcall, :block_given?)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
    end
  end

  it "compiles 'iterator?'" do
    ruby = <<-EOC
      iterator?
    EOC

    sexp = s(:fcall, :block_given?)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
    end
  end

  it "compiles 'self.blah(1)'" do
    ruby = <<-EOC
      self.blah(1)
    EOC

    sexp = s(:call, s(:self), :blah, s(:array, s(:fixnum, 1)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.push 1
      g.send :blah, 1, false
    end
  end

  it "compiles '1.go(2)'" do
    ruby = <<-EOC
      1.go(2)
    EOC

    sexp = s(:call, s(:fixnum, 1), :go, s(:array, s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.send :go, 1, false
    end
  end

  it "compiles '10.times(2) { 12 }'" do
    ruby = <<-EOC
      10.times(2) { 12 }
    EOC

    sexp = s(:iter,
             s(:call, s(:lit, 10), :times, s(:array, s(:fixnum, 2))),
             nil,
             s(:lit, 12))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.pop_modifiers
        d.ret
      end

      g.push 10
      g.push 2
      g.create_block desc
      g.passed_block do
        g.send_with_block :times, 1, false
      end
    end
  end

  it "compiles 'a.b(*c) { 12 }'" do
    ruby = <<-EOC
      a.b(*c) { 12 }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :a), :b, s(:splat, s(:vcall, :c))),
             nil,
             s(:lit, 12))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.pop
        d.push_modifiers
        d.new_label.set! # redo
        d.push 12
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :a, 0, true

      g.push :self
      g.send :c, 0, true
      g.cast_array

      g.create_block desc

      g.passed_block do
        g.send_with_splat :b, 0, false, false
      end
    end
  end

  it "compiles 'h[:blah] = 8'" do
    ruby = <<-EOC
      h[:blah] = 8
    EOC

    sexp = s(:attrasgn, s(:vcall, :h), :[]=,
             s(:array, s(:lit, :blah), s(:fixnum, 8)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :h, 0, true
      g.push_unique_literal :blah
      g.push 8
      g.send :[]=, 2, false
    end
  end

  it "compiles 'a.b = 8'" do
    ruby = <<-EOC
      a.b = 8
    EOC

    sexp = s(:attrasgn, s(:vcall, :a), :b, s(:array, s(:fixnum, 8)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :a, 0, true
      g.push 8
      g.send :b=, 1, false
    end
  end

  it "compiles 'self[index, 0] = other_string'" do
    ruby = <<-EOC
      self[index, 0] = other_string
    EOC

    sexp = s(:attrasgn, nil, :[]=,
             s(:array,
               s(:vcall, :index),
               s(:fixnum, 0),
               s(:vcall, :other_string)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.push :self
      g.send :index, 0, true
      g.push 0
      g.push :self
      g.send :other_string, 0, true
      g.send :[]=, 3, false
    end
  end

  # Dynamic argument count specs
  it "compiles 'h(1, 2, *a)'" do
    ruby = <<-EOC
      h(1, 2, *a)
    EOC

    sexp = s(:fcall, :h,
             s(:argscat,
               s(:array, s(:fixnum, 1), s(:fixnum, 2)),
               s(:vcall, :a)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.push 1
      g.push 2
      g.push :self
      g.send :a, 0, true
      g.cast_array

      g.push :nil
      g.send_with_splat :h, 2, true, false
    end
  end

  it "compiles 'f[*x] = 3'" do
    ruby = <<-EOC
      f[*x] = 3
    EOC

    sexp = s(:attrasgn, s(:vcall, :f), :[]=,
             s(:argspush, s(:splat, s(:vcall, :x)), s(:fixnum, 3)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :f, 0, true
      g.push :self
      g.send :x, 0, true
      g.cast_array

      g.push 3
      g.swap
      g.push :nil
      g.send_with_splat :[]=, 1, false, true
    end
  end

  it "compiles 'undef :blah'" do
    ruby = <<-EOC
      undef :blah
    EOC

    sexp = s(:undef, :blah)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :metaclass, 0
      g.push_literal :blah
      g.send :undef_method, 1
    end
  end

  it "compiles 'class B; undef :blah; end'" do
    ruby = <<-EOC
      class B; undef :blah; end
    EOC

      sexp = s(:class, s(:colon2, :A), nil, s(:scope, s(:undef, :blah), s()))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.push_self # FIX
        d.add_scope
        d.push :self
        d.push_literal :blah
        d.send :undef_method, 1
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

  it "compiles 'yield'" do
    ruby = <<-EOC
      yield
    EOC

      sexp = s(:yield, nil, false)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
      g.meta_send_call 0
    end
  end

  it "compiles 'yield 1'" do
    ruby = <<-EOC
      yield 1
    EOC

      sexp = s(:yield, s(:fixnum, 1), false)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
      g.push 1
      g.meta_send_call 1
    end
  end

  it "compiles 'yield 1, 2'" do
    ruby = <<-EOC
      yield 1, 2
    EOC

      sexp = s(:yield, s(:array, s(:fixnum, 1), s(:fixnum, 2)), true)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
      g.push 1
      g.push 2
      g.meta_send_call 2
    end
  end

  it "compiles 'yield [1, 2]'" do
    ruby = <<-EOC
      yield [1, 2]
    EOC

      sexp = s(:yield, s(:array, s(:fixnum, 1), s(:fixnum, 2)), false)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
      g.push 1
      g.push 2
      g.make_array 2
      g.meta_send_call 1
    end
  end

  it "compiles 'yield *a'" do
    ruby = <<-EOC
      yield *a
    EOC

      sexp = s(:yield, s(:splat, s(:vcall, :a)), false)

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push_block
      g.push :self
      g.send :a, 0, true
      g.cast_array
      g.push :nil
      g.send_with_splat :call, 0, false, false
    end
  end

  it "compiles 'super(1)'" do
    ruby = <<-EOC
      def a
        super(1)
      end
    EOC

      sexp = s(:defn, :a,
               s(:scope,
                 s(:block,
                   s(:args),
                   s(:super, s(:array, s(:fixnum, 1)))), s()))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.push 1
        d.push :nil
        d.push :nil
        d.send_super :a, 1
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'super(*blah)'" do
    ruby = <<-EOC
      def a
        super(*blah)
      end
    EOC

      sexp = s(:defn, :a,
               s(:scope,
                 s(:block,
                   s(:args),
                   s(:super, s(:splat, s(:vcall, :blah)))), s()))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.push :self
        d.send :blah, 0, true
        d.cast_array
        d.push :nil
        d.send_super :a, 0, true
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'super'" do
    ruby = <<-EOC
      def a(a, b)
        super
      end
    EOC

      sexp = s(:defn, :a,
               s(:scope,
                 s(:block,
                   s(:args, s(:a, :b), s(), nil, nil),
                   s(:zsuper)), s()))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.push_local 0
        d.push_local 1
        d.push :nil
        d.push :nil
        d.send_super :a, 2
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'super' with a splat in the arg list" do
    ruby = <<-EOC
      def a(name, *rest)
        super
      end
    EOC

      sexp = s(:defn, :a,
               s(:scope,
                 s(:block,
                   s(:args, s(:name), s(), s(:rest, 1), nil),
                   s(:zsuper)), s()))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.push_local 0
        d.push_local 1
        d.cast_array
        d.push :nil
        d.send_super :a, 1, true
        d.ret
      end

      g.push :self
      g.push_literal :a
      g.push_literal desc
      g.send :__add_method__, 2
    end
  end

  it "compiles 'foo(&blah)'" do
    ruby = <<-EOC
      foo(&blah)
    EOC

    sexp = s(:block_pass, s(:vcall, :blah), s(:fcall, :foo))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      lbl = g.new_label

      g.push :self
      g.push_cpath_top
      g.find_const :Proc

      g.push :self
      g.send :blah, 0, true
      g.dup
      g.is_nil
      g.git lbl
      g.send :__from_block__, 1
      lbl.set!
      g.send_with_block :foo, 0, true
    end
  end

  it "compiles 'END { 666 }'" do
    ruby = <<-EOC
      END { 666 }
    EOC

      sexp = s(:iter, s(:postexe), nil, s(:lit, 666))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description :__block__ do |d|
        redo_label = d.new_label

        d.pop
        d.push_modifiers
        redo_label.set!
        d.push 666
        d.pop_modifiers
        d.ret
      end

      desc.required = -1

      g.push :self
      g.create_block desc
      g.passed_block do
        g.send_with_block :at_exit, 0, true
      end
    end
  end
end
