require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles 'a, b = 1, 2'" do
    # TODO to ryan: [:masgn, [:array, ..], nil, [:array, ..]]
    ruby = <<-EOC
      a, b = 1, 2
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b)),
             s(:array, s(:fixnum, 1), s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2

      g.rotate 2

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop

      g.push :true
    end
  end

  it "compiles 'a, b.c = b.c, true'" do
    # TODO to ryan: same as above
    ruby = <<-EOC
      a, b.c = b.c, true
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:attrasgn, s(:vcall, :b), :c)),
             s(:array, s(:call, s(:vcall, :b), :c), s(:true)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :b, 0, true
      g.send :c, 0, false
      g.push :true

      g.rotate 2

      g.set_local 0
      g.pop

      g.push :self
      g.send :b, 0, true
      g.send :c=, 1, false

      g.pop

      g.push :true
    end
  end

  it "compiles 'a, b = 1, 2, 3'" do
    # TODO to ryan: same as above
    ruby = <<-EOC
      a, b = 1, 2, 3
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b)),
             s(:array, s(:fixnum, 1), s(:fixnum, 2), s(:fixnum, 3)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.push 3

      g.rotate 3

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop

      g.pop # no set_local since the LHS is smaller than the RHS
      g.push :true
    end
  end

  it "compiles 'a, b, c = 1, 2'" do
    ruby = <<-EOC
      a, b, c = 1, 2
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:lasgn, :c)),
             s(:array, s(:fixnum, 1), s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :nil
      g.push 1
      g.push 2

      g.rotate 2

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop
      g.set_local 2
      g.pop

      g.push :true
    end
  end

  it "compiles 'a, *b = 1, 2, 3'" do
    ruby = <<-EOC
      a, *b = 1, 2, 3
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a)), s(:lasgn, :b),
             s(:array, s(:fixnum, 1), s(:fixnum, 2), s(:fixnum, 3)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.push 3

      g.make_array 2

      g.set_local 1
      g.pop

      g.set_local 0
      g.pop

      g.push :true
    end
  end

  it "compiles 'a, b, *c = 1, 2, 3'" do
    ruby = <<-EOC
      a, b, *c = 1, 2, 3
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b)), s(:lasgn, :c),
             s(:array, s(:fixnum, 1), s(:fixnum, 2), s(:fixnum, 3)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.push 3

      g.make_array 1
      g.set_local 2
      g.pop

      g.set_local 1
      g.pop

      g.set_local 0
      g.pop

      g.push :true
    end
  end

  it "compiles 'a, b, c = *d'" do
    # TODO to ryan: [:masgn, [:array, ...], nil, nil, [:vcall, :d]]
    ruby = <<-EOC
      a, b, c = *d
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:lasgn, :c)),
             s(:splat, s(:vcall, :d)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :d, 0, true

      g.cast_tuple

      g.lvar_set 0
      g.lvar_set 1
      g.lvar_set 2

      g.pop
      g.push :true
    end
  end

  it "compiles 'a, b, c = 1, *d'" do
    # TODO to ryan: [:masgn, [:array, ...], nil, [:array, [:lit, 1]], [:vcall, :d]]
    ruby = <<-EOC
      a, b, c = 1, *d
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:lasgn, :c)),
             s(:argscat, s(:array, s(:lit, 1)), s(:vcall, :d)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.make_array 1

      g.push :self
      g.send :d, 0, true
      g.cast_array

      g.send :+, 1
      g.cast_tuple

      g.lvar_set 0
      g.lvar_set 1
      g.lvar_set 2

      g.pop
      g.push :true
    end
  end

  it "compiles 'a, b, *c = *d'" do
    # TODO to ryan: [:masgn, [:array, ...], [:vcall, :c], nil, [:vcall, :d]]
    ruby = <<-EOC
      a, b, *c = *d
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b)), s(:lasgn, :c),
             s(:splat, s(:vcall, :d)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :d, 0, true

      g.cast_tuple

      g.lvar_set 0
      g.lvar_set 1

      g.cast_array
      g.set_local 2
      g.pop

      g.push :true
    end
  end

  it "compiles 'x.each { |a| }'" do
    ruby = <<-EOC
      x.each { |a| }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :x), :each),
             s(:lasgn, :a))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.cast_for_single_block_arg
        d.set_local_depth 0,0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block desc
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles 'x.each { |a,| }'" do
    ruby = <<-EOC
      x.each { |a,| }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :x), :each),
             s(:masgn, s(:array, s(:lasgn, :a))))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local_depth 0,0
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block desc
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles 'x.each { |a,b| }'" do
    ruby = <<-EOC
      x.each { |a,b| }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :x), :each),
             s(:masgn, s(:array,
                         s(:lasgn, :a),
                         s(:lasgn, :b))))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local_depth 0,0
        d.pop
        d.shift_tuple
        d.set_local_depth 0,1

        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block desc
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles 'x.each { |*args| }'" do
    # TODO to ryan: [:masgn, nil, [:lasgn, :args]]
    ruby = <<-EOC
      x.each { |*args| }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :x), :each),
             s(:masgn, s(:lasgn, :args)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.cast_array
        d.set_local_depth 0, 0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block desc
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end


  it "compiles 'x.each { |a, *b| }'" do
    ruby = <<-EOC
      x.each { |a, *b| }
    EOC

    sexp = s(:iter,
             s(:call, s(:vcall, :x), :each),
             s(:masgn, s(:array, s(:lasgn, :a)), s(:lasgn, :b)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.shift_tuple
        d.set_local_depth 0,0
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.ret
      end

      g.push :self
      g.send :x, 0, true
      g.create_block desc
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end

  it "compiles '@a, @b = 1, 2'" do
    ruby = <<-EOC
      @a, @b = 1, 2
    EOC

    sexp = s(:masgn,
             s(:array, s(:iasgn, :@a), s(:iasgn, :@b)),
             s(:array, s(:fixnum, 1), s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.rotate 2
      g.set_ivar :@a
      g.pop
      g.set_ivar :@b
      g.pop
      g.push :true
    end
  end

  it "compiles '@a, $b = 1, 2'" do
    ruby = <<-EOC
      @a, $b = 1, 2
    EOC

    sexp = s(:masgn,
             s(:array, s(:iasgn, :@a), s(:gasgn, :$b)),
             s(:array, s(:fixnum, 1), s(:fixnum, 2)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push 1
      g.push 2
      g.rotate 2
      g.set_ivar :@a
      g.pop
      g.push_cpath_top
      g.find_const :Globals
      g.swap
      g.push_literal :$b
      g.swap
      g.send :[]=, 2
      g.pop
      g.push :true
    end
  end

  it "compiles 'a, b = (@a = 1), @a'" do
    ruby = <<-EOC
      a, b = (@a = 1), @a
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :a), s(:lasgn, :b)),
             s(:array,
               s(:iasgn, :@a, s(:lit, 1)),
               s(:ivar, :@a)))

    sexp.should == parse(ruby) if $unified && $new

    gen(sexp) do |g|
      g.push 1
      g.set_ivar :@a
      g.push_ivar :@a

      g.rotate 2

      g.set_local 0
      g.pop
      g.set_local 1
      g.pop

      g.push :true
    end
  end

  it "compiles 'entry, hash, bin = hash_entry key'" do
    ruby = <<-EOC
      entry, hash, bin = hash_entry key
    EOC

    sexp = s(:masgn,
             s(:array, s(:lasgn, :entry), s(:lasgn, :hash), s(:lasgn, :bin)),
             s(:to_ary, s(:fcall, :hash_entry, s(:array, s(:vcall, :key)))))

    sexp.should == parse(ruby) if $unified && $new

    gen(sexp) do |g|
      g.push :self
      g.push :self
      g.send :key, 0, true
      g.send :hash_entry, 1, true
      g.cast_tuple
      g.shift_tuple
      g.set_local 0
      g.pop
      g.shift_tuple
      g.set_local 1
      g.pop
      g.shift_tuple
      g.set_local 2
      g.pop
      g.pop
      g.push :true
    end
  end
end
