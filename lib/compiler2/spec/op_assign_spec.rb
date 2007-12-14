require File.dirname(__FILE__) + "/helper.rb"

describe Compiler2 do
  it "compiles 'a ||= 8'" do
    x = [:op_asgn_or, [:lvar, :a, 0], [:lasgn, :a, 0, [:lit, 8]]]
    gen x do |g|
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
    x = [:op_asgn_and, [:lvar, :a, 0], [:lasgn, :a, 0, [:lit, 8]]]
    gen x do |g|
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
    x = [:op_asgn1, [:vcall, :hsh], :or,
          [:array, [:lit, 8], [:lit, :blah], [:nil]]]
          
    gen x do |g|
      found = g.new_label
      fin = g.new_label
      
      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_literal :blah
      g.swap
      g.send :[], 1
      g.dup
      g.git found
      
      g.pop
      g.push 8
      g.swap
      g.push_literal :blah
      g.swap
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
    x = [:op_asgn1, [:vcall, :hsh], :and,
          [:array, [:lit, 8], [:lit, :blah], [:nil]]]
          
    gen x do |g|
      found = g.new_label
      fin = g.new_label
      
      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_literal :blah
      g.swap
      g.send :[], 1
      g.dup
      g.gif found
      
      g.pop
      g.push 8
      g.swap
      g.push_literal :blah
      g.swap
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
    x = [:op_asgn1, [:vcall, :hsh], :"^",
          [:array, [:lit, 8], [:lit, :blah], [:nil]]]
          
    gen x do |g|
      fin = g.new_label
      
      g.push :self
      g.send :hsh, 0, true
      g.dup
      g.push_literal :blah
      g.swap
      g.send :[], 1
      g.push 8
      g.swap
      g.send :"^", 1
      g.swap
      g.push_literal :blah
      g.swap
      g.send :[]=, 2
    end
    
  end
  
  it "compiles 'ary[0,1] += [4]'" do
    x = [:op_asgn1, [:vcall, :ary], :+, 
          [:array, [:array, [:lit, 4]], [:fixnum, 0], [:lit, 1], [:nil]]
        ]
    gen x do |g|
      g.push :self
      g.send :ary, 0, true
      g.dup
      g.push 1
      g.swap
      g.push 0
      g.swap
      g.send :[], 2
      g.push 4
      g.make_array 1
      g.swap
      g.send :"+", 1
      g.swap
      g.push 1
      g.swap
      g.push 0
      g.swap
      g.send :[]=, 3
    end
  end
  
  it "compiles 'x.val ||= 6'" do
    x = [:op_asgn2, [:vcall, :x], :val, :or, :val, [:lit, 6]]
    
    gen x do |g|
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
      g.swap
      g.send :val=, 1
      g.goto fin
      
      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end
  
  it "compiles 'x.val &&= 7" do
    x = [:op_asgn2, [:vcall, :x], :val, :and, :val, [:lit, 7]]
    
    gen x do |g|
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
      g.swap
      g.send :val=, 1
      g.goto fin
      
      fnd.set!
      g.swap
      g.pop
      fin.set!
    end
  end
  
  it "compiles 'x.val ^= 8" do
    x = [:op_asgn2, [:vcall, :x], :val, :^, :val, [:lit, 8]]
    
    gen x do |g|
      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :val, 0
      g.push 8
      g.swap
      g.send :"^", 1
      g.swap
      g.send :val=, 1
    end
  end
  
end