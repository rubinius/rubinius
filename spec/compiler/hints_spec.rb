require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles '1 + 1' using fastmath" do
    ruby = <<-EOC
      1 + 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :+, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_plus
    end
  end

  it "compiles '1 - 1' using fastmath" do
    ruby = <<-EOC
      1 - 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :-, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_minus
    end
  end

  it "compiles '1 == 1' using fastmath" do
    ruby = <<-EOC
      1 == 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :==, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_equal
    end
  end

  it "compiles '1 === 1' using fastmath" do
    ruby = <<-EOC
      1 === 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :===, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_tequal
    end
  end

  it "compiles '1 < 1' using fastmath" do
    ruby = <<-EOC
      1 < 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :<, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_lt
    end
  end

  it "compiles '1 > 1' using fastmath" do
    ruby = <<-EOC
      1 > 1
    EOC

    sexp = s(:call, s(:fixnum, 1), :>, s(:arglist, s(:fixnum, 1)))

    sexp.should == parse(ruby)

    gen sexp, [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_gt
    end
  end

  it "compiles '4 / 2' using :/ without safemath" do
    ruby = <<-EOC
      4 / 2
    EOC

    sexp = s(:call, s(:fixnum, 4), :/, s(:arglist, s(:fixnum, 2)))

    sexp.should == parse(ruby)

    gen sexp do |g|
      g.push 4
      g.push 2
      g.send :/, 1, false
    end
  end

  it "compiles '4 / 2' using :divide with safemath" do
    ruby = <<-EOC
      4 / 2
    EOC

    sexp = s(:call, s(:fixnum, 4), :/, s(:arglist, s(:fixnum, 2)))

    sexp.should == parse(ruby)

    gen sexp, [:safemath] do |g|
      g.push 4
      g.push 2
      g.send :divide, 1, false
    end
  end
end
