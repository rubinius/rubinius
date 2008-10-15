require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles '[1, 2, *foo()]" do
    ruby = <<-EOC
      [1, 2, *foo()]
    EOC

    sexp = s(:array,
             s(:fixnum, 1),
             s(:fixnum, 2),
             s(:splat, s(:call, nil, :foo, s(:arglist))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 1
      g.push 2
      g.make_array 2

      g.push :self
      g.send :foo, 0, true
      g.cast_array

      g.send :+, 1
    end
  end

  it "coerces the array reference and concatenates it to the first array" do
    ruby = <<-EOC
      x = [42]
      [1, 2, *x]
    EOC

    sexp = s(:block,
             s(:lasgn, :x, s(:array, s(:fixnum, 42))),
             s(:array,
               s(:fixnum, 1),
               s(:fixnum, 2),
               s(:splat, s(:lvar,  :x))))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 42
      g.make_array 1
      g.set_local 0
      g.pop

      g.push 1
      g.push 2
      g.make_array 2

      g.push_local 0
      g.cast_array

      g.send :+, 1
    end
  end

end
