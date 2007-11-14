require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  it "compiles '1 + 1' using fastmath" do
    gen [:call, [:fixnum, 1], :+, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_plus
    end
  end
  
  it "compiles '1 - 1' using fastmath" do
    gen [:call, [:fixnum, 1], :-, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_minus
    end
  end
  
  it "compiles '1 == 1' using fastmath" do
    gen [:call, [:fixnum, 1], :==, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_equal
    end
  end
  
  it "compiles '1 != 1' using fastmath" do
    gen [:call, [:fixnum, 1], :"!=", [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_nequal
    end
  end

  it "compiles '1 === 1' using fastmath" do
    gen [:call, [:fixnum, 1], :===, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_tequal
    end
  end

  it "compiles '1 < 1' using fastmath" do
    gen [:call, [:fixnum, 1], :<, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_lt
    end
  end

  it "compiles '1 > 1' using fastmath" do
    gen [:call, [:fixnum, 1], :>, [:array, [:fixnum, 1]]], [:fastmath] do |g|
      g.push 1
      g.push 1
      g.meta_send_op_gt
    end
  end
  
  it "detects Ruby.primitive only as the first statement" do
    gen [:call, [:const, :Ruby], :primitive, [:array, [:lit, :blah]]] do |g|
      g.as_primitive :blah
    end
        
    x = [:block, 
           [:true], 
           [:call, [:const, :Ruby], :primitive, [:array, [:lit, :blah]]]]
    
    gen x do |g|
      g.push :true
      g.pop
      g.push_literal :blah
      g.push_const :Ruby
      g.send :primitive, 1, false
    end
    
  end
  
  
end
