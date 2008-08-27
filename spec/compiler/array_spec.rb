require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles '[1, 2, *foo()]" do
    ruby = <<-EOC
      [1, 2, *foo()]
    EOC

    sexp = s(:argscat,
             s(:array, s(:lit, 1), s(:lit, 2)),
             s(:fcall, :foo))

    sexp.should == parse(ruby) if $unified && $new

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
             s(:lasgn, :x, s(:array, s(:lit, 42))),
             s(:argscat,
               s(:array, s(:lit, 1), s(:lit, 2)),
               s(:lvar,  :x, 0)))

    sexp.should == parse(ruby) if $unified && $new

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
