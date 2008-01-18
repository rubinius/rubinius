require File.dirname(__FILE__) + "/spec_helper"

describe Compiler do
  it "compiles 'a, b = 1, 2'" do
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b]], nil,
          [:array, [:fixnum, 1], [:fixnum, 2]]]

    gen x do |g|
      g.push 2
      g.push 1
      g.set_local 0
      g.pop
      g.set_local 1
      g.pop
      g.push :true
    end
  end
  
  it "compiles 'a, b.c = b.c, true'" do
    x = [:masgn, 
          [:array, [:lasgn, :a], [:attrasgn, [:vcall, :b], :c]], nil, 
          [:array, [:call, [:vcall, :b], :c], [:true]]
        ]
        
    gen x do |g|
      g.push :true
      g.push :self
      g.send :b, 0, true
      g.send :c, 0, false
      
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
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b]], nil,
          [:array, [:fixnum, 1], [:fixnum, 2], [:fixnum, 3]]]

    gen x do |g|
      g.push 3
      g.push 2
      g.push 1
      g.set_local 0
      g.pop
      g.set_local 1
      g.pop
      g.pop
      g.push :true
    end
  end
  
  it "compiles 'a, b, c = 1, 2'" do
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b], [:lasgn, :c]], nil,
          [:array, [:fixnum, 1], [:fixnum, 2]]]

    gen x do |g|
      g.push :nil
      g.push 2
      g.push 1
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
    x = [:masgn, 
          [:array, [:lasgn, :a]], [:lasgn, :b],
          [:array, [:fixnum, 1], [:fixnum, 2], [:fixnum, 3]]]

    gen x do |g|
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
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b]], [:lasgn, :c],
          [:array, [:fixnum, 1], [:fixnum, 2], [:fixnum, 3]]]

    gen x do |g|
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
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b], [:lasgn, :c]], nil, 
          [:splat, [:vcall, :d]]]
    
    gen x do |g|
      g.push :self
      g.send :d, 0, true
      g.cast_tuple
      
      g.unshift_tuple
      g.set_local 0
      g.pop
      
      g.unshift_tuple
      g.set_local 1
      g.pop
      
      g.unshift_tuple
      g.set_local 2
      g.pop
      
      g.pop
      g.push :true
    end
  end
  
  it "compiles 'a, b, c = 1, *d'" do
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b], [:lasgn, :c]], nil, 
          [:argscat, [:array, [:lit, 1]], [:vcall, :d]]
        ]
    
    gen x do |g|
      g.push :self
      g.send :d, 0, true
      g.cast_array
      g.push 1
      g.make_array 1
      g.send :+, 1
      g.cast_tuple
      
      g.unshift_tuple
      g.set_local 0
      g.pop
      
      g.unshift_tuple
      g.set_local 1
      g.pop
      
      g.unshift_tuple
      g.set_local 2
      g.pop
      
      g.pop
      g.push :true
      
    end
  end
  
  it "compiles 'a, b, *c = *d'" do
    x = [:masgn, 
          [:array, [:lasgn, :a], [:lasgn, :b]], [:lasgn, :c], 
          [:splat, [:vcall, :d]]
        ]
    
    gen x do |g|
      g.push :self
      g.send :d, 0, true
      g.cast_tuple
      
      g.unshift_tuple
      g.set_local 0
      g.pop
      
      g.unshift_tuple
      g.set_local 1
      g.pop
      
      g.cast_array
      g.set_local 2
      g.pop
      
      g.push :true
      
    end
  end
  
  it "compiles '|a|'" do
    x = [:iter_args, [:lasgn, :a]]
    
    gen x do |g|
      g.cast_for_single_block_arg
      g.set_local 0
      g.pop
    end
  end
  
  it "compiles '|a,|'" do
    x = [:iter,
         [:call, [:vcall, :x], :each], 
         [:masgn, [:array, [:lasgn, :a]], nil, nil]]
    
    gen x do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.unshift_tuple
        d.set_local_depth 0,0
        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.soft_return
      end

      g.push_literal desc
      g.create_block2
      g.push :self
      g.send :x, 0, true
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end
  
  it "compiles '|a,b|'" do
    x = [:iter,
         [:call, [:vcall, :x], :each], 
         [:masgn, [:array,
                   [:lasgn, :a],
                   [:lasgn, :b]], nil, nil]]

    gen(x) do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.unshift_tuple
        d.set_local_depth 0,0
        d.pop
        d.unshift_tuple
        d.set_local_depth 0,1

        d.pop
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.soft_return
      end

      g.push_literal desc
      g.create_block2
      g.push :self
      g.send :x, 0, true
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end
  
  it "compiles '|*args|'" do
    x = [:iter,
         [:call, [:vcall, :x], :each], 
         [:masgn, [:lasgn, :args], nil]]
    
    gen x do |g|
      desc = description do |d|
        d.cast_array
        d.set_local_depth 0, 0
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.soft_return
      end

      g.push_literal desc
      g.create_block2
      g.push :self
      g.send :x, 0, true
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end
 

  it "compiles '|a, *b|'" do
    x = [:iter, [:call, [:vcall, :x], :each], 
          [:masgn, [:array, [:lasgn, :a]], [:lasgn, :b], nil]
        ]
    
    gen x do |g|
      desc = description do |d|
        d.cast_for_multi_block_arg
        d.unshift_tuple
        d.set_local_depth 0,0
        d.pop
        d.cast_array
        d.set_local_depth 0,1
        d.pop
        d.push_modifiers
        d.new_label.set!
        d.push :nil
        d.pop_modifiers
        d.soft_return
      end

      g.push_literal desc
      g.create_block2
      g.push :self
      g.send :x, 0, true
      g.passed_block do
        g.send_with_block :each, 0, false
      end
    end
  end
  
  it "compiles '@a, @b = 1, 2'" do
    x = [:masgn, 
          [:array, [:iasgn, :@a], [:iasgn, :@b]], nil,
          [:array, [:fixnum, 1], [:fixnum, 2]]]

    gen x do |g|
      g.push 2
      g.push 1
      g.set_ivar :@a
      g.pop
      g.set_ivar :@b
      g.pop
      g.push :true
    end
  end
  
  it "compiles '@a, $b = 1, 2'" do
    x = [:masgn, 
          [:array, [:iasgn, :@a], [:gasgn, :$b]], nil,
          [:array, [:fixnum, 1], [:fixnum, 2]]]

    gen x do |g|
      g.push 2
      g.push 1
      g.set_ivar :@a
      g.pop
      g.push_literal :$b
      g.push_cpath_top
      g.find_const :Globals
      g.send :[]=, 2
      g.pop
      g.push :true
    end
  end
  
end
