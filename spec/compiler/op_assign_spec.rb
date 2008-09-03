require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles 'a ||= 8'" do
    ruby = <<-EOC
      a ||= 8
    EOC

    sexp = s(:op_asgn_or, s(:lvar, :a), s(:lasgn, :a, s(:fixnum, 8)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      fin = g.new_label

      g.push_local 0
      g.dup
      g.git fin

      g.pop
      g.push 8
      g.set_local 0

      fin.set!
    end
  end

  it "compiles 'a &&= 8'" do
    ruby = <<-EOC
      a &&= 8
    EOC

    sexp = s(:op_asgn_and, s(:lvar, :a), s(:lasgn, :a, s(:fixnum, 8)))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      fin = g.new_label

      g.push_local 0
      g.dup
      g.gif fin

      g.pop
      g.push 8
      g.set_local 0

      fin.set!
    end
  end

  it "compiles 'hsh[:blah] ||= 8'" do
    ruby = <<-EOC
      hsh[:blah] ||= 8
    EOC

    sexp = s(:op_asgn1,
             s(:call, nil, :hsh, s(:arglist)),
             s(:arglist, s(:lit, :blah)),
             :"||",
             s(:fixnum, 8))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      found = g.new_label
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_unique_literal :blah
      g.send :[], 1
      g.dup
      g.git found

      g.pop
      g.push_unique_literal :blah
      g.push 8
      g.send :[]=, 2
      g.goto fin

      found.set!

      # Remove the object from the stack
      g.swap
      g.pop

      fin.set!
    end
  end

  it "compiles 'hsh[:blah] &&= 8'" do
    ruby = <<-EOC
      hsh[:blah] &&= 8
    EOC

    sexp = s(:op_asgn1,
             s(:call, nil, :hsh, s(:arglist)),
             s(:arglist, s(:lit, :blah)),
             :"&&",
             s(:fixnum, 8))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      found = g.new_label
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_unique_literal :blah
      g.send :[], 1
      g.dup
      g.gif found

      g.pop
      g.push_unique_literal :blah
      g.push 8
      g.send :[]=, 2
      g.goto fin

      found.set!

      # Remove the object from the stack
      g.swap
      g.pop

      fin.set!
    end

  end

  it "compiles 'hsh[:blah] ^= 8'" do
    ruby = <<-EOC
      hsh[:blah] ^= 8
    EOC

    sexp = s(:op_asgn1,
             s(:call, nil, :hsh, s(:arglist)),
             s(:arglist, s(:lit, :blah)),
             :^,
             s(:fixnum, 8))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      fin = g.new_label

      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_unique_literal :blah
      g.send :[], 1
      g.push 8
      g.send :"^", 1
      g.push_unique_literal :blah
      g.swap
      g.send :[]=, 2
    end

  end

  it "compiles 'ary[0,1] += [4]'" do
    ruby = <<-EOC
      ary[0,1] += [4]
    EOC

    sexp = s(:op_asgn1,
             s(:call, nil, :ary, s(:arglist)),
             s(:arglist, s(:fixnum, 0), s(:fixnum, 1)),
             :+,
             s(:array, s(:fixnum, 4)))

    gen sexp do |g|
    sexp.should == parse(ruby) if $unified && $new

      g.push :self
      g.send :ary, 0, true
      g.dup
      g.push 0
      g.push 1
      g.send :[], 2
      g.push 4
      g.make_array 1
      g.send :"+", 1
      g.push 0
      g.swap
      g.push 1
      g.swap
      g.send :[]=, 3
    end
  end

  it "compiles 'x.val ||= 6'" do
    ruby = <<-EOC
      x.val ||= 6
    EOC

    sexp = s(:op_asgn2,
             s(:call, nil, :x, s(:arglist)),
             :val=, :"||",
             s(:fixnum, 6))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      fnd = g.new_label
      fin = g.new_label

      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.dup
      g.git fnd

      g.pop
      g.push 6
      g.send :val=, 1
      g.goto fin

      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end

  it "compiles 'x.val &&= 7'" do
    ruby = <<-EOC
      x.val &&= 7
    EOC

    sexp = s(:op_asgn2, s(:call, nil, :x, s(:arglist)), :val=, :"&&", s(:fixnum, 7))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      fnd = g.new_label
      fin = g.new_label

      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.dup
      g.gif fnd

      g.pop
      g.push 7
      g.send :val=, 1
      g.goto fin

      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end

  it "compiles 'x.val ^= 8'" do
    ruby = <<-EOC
      x.val ^= 8
    EOC

    sexp = s(:op_asgn2, s(:call, nil, :x, s(:arglist)), :val=, :^, s(:fixnum, 8))

    sexp.should == parse(ruby) if $unified && $new

    gen sexp do |g|
      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.push 8
      g.send :"^", 1
      g.send :val=, 1
    end
  end
end
