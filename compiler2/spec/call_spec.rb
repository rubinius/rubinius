require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  
  it "compiles 'self.blah(1)'" do
    x = [:call, [:self], :blah, [:array, [:fixnum, 1]]]
    
    gen x do |g|
      g.push 1
      g.push :self
      g.send :blah, 1, false
    end
  end
  
  it "compiles 'h[:blah] = 8'" do
    x = [:attrasgn, [:vcall, :h], :[]=, [:array, [:lit, :blah], [:fixnum, 8]]]
    
    gen x do |g|
      g.push 8
      g.push_literal :blah
      g.push :self
      g.send :h, 0, true
      g.send :[]=, 2, false
    end
  end
  
  # Dynamic argument count specs
  it "compiles 'h(1, 2, *a)'" do
    x = [:fcall, :h, [:argscat, [:array, [:fixnum, 1], [:fixnum, 2]],
            [:vcall, :a]]]
    gen x do |g|
      g.push :self
      g.send :a, 0, true
      g.cast_array_for_args 2
      g.push_array
      g.push 2
      g.push 1
      g.push :self
      g.send_with_register :h, true
    end
  end
  
  it "compiles 'f[*x] = 3'" do
    x = [:attrasgn, [:vcall, :f], :[]=, 
        [:argspush, [:splat, [:vcall, :x]], [:fixnum, 3]]]
        
    gen x do |g|
      g.push 3
      g.push :self
      g.send :x, 0, true
      g.cast_array_for_args 1
      g.push_array
      g.push :self
      g.send :f, 0, true
      g.send_with_register :[]=, false
    end
  end
end
