require File.dirname(__FILE__) + "/helper.rb"

describe Compiler2 do
  
  it "compiles 'blah'" do |g|
    gen [:vcall, :blah] do |g|
      g.push :self
      g.send :blah, 0, true
    end
  end
  
  it "compiles 'blah()'" do
    gen [:fcall, :blah] do |g|
      g.push :self
      g.send :blah, 0, true 
    end
  end
  
  it "compiles 'blah(1,2)'" do
    gen [:fcall, :blah, [:array, [:fixnum, 1], [:fixnum, 2]]] do |g|
      g.push 2
      g.push 1
      g.push :self
      g.send :blah, 2, true 
    end
  end
  
  it "compiles 'blah(*a)'" do
    gen [:fcall, :blah, [:splat, [:vcall, :a]]] do |g|
      g.push :self
      g.send :a, 0, true
      g.cast_array_for_args 0
      g.push_array
      g.get_args
      g.push :nil
      g.swap
      g.push :self
      g.swap
      g.set_args
      g.send_with_register :blah, true
    end
  end
  
  it "compiles 'block_given?'" do
    gen [:fcall, :block_given?] do |g|
      g.push_block
    end
  end
  
  it "compiles 'iterator?'" do
    gen [:fcall, :block_given?] do |g|
      g.push_block
    end
  end
  
  
  it "compiles 'self.blah(1)'" do
    x = [:call, [:self], :blah, [:array, [:fixnum, 1]]]
    
    gen x do |g|
      g.push 1
      g.push :self
      g.send :blah, 1, false
    end
  end
  
  it "compiles '1.go(2)'" do
    x = [:call, [:fixnum, 1], :go, [:array, [:fixnum, 2]]]
    
    gen x do |g|
      g.push 2
      g.push 1
      g.send :go, 1, false 
    end
  end
  
  it "compiles '10.times(2) { 12 }'" do
    x = [:iter, 
          [:call, [:lit, 10], :times, [:array, [:fixnum, 2]]], 
          nil, 
          [:lit, 12]
        ]
    
    gen x do |g|
      desc = description do |d|
        d.pop
        d.new_label.set! # redo
        d.push 12
        d.soft_return
      end
      
      g.push 2
      g.push_literal desc
      g.create_block2
      g.push 10
      g.send_with_block :times, 1, false
    end
  end
  
  it "compiles 'a.b(*c) { 12 }'" do
    x = [:iter, 
          [:call, [:vcall, :a], :b, [:splat, [:vcall, :c]]], 
          nil, 
          [:lit, 12]
        ]

    gen x do |g|
       desc = description do |d|
         d.pop
         d.new_label.set! # redo
         d.push 12
         d.soft_return
       end
       g.push :self
       g.send :c, 0, true
       g.cast_array_for_args 0
       g.push_array
       g.get_args
       g.push_literal desc
       g.create_block2
       g.swap
       g.push :self
       g.send :a, 0, true
       g.swap
       g.set_args
       g.send_with_register :b, false
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
  
  it "compiles 'a.b = 8'" do
    x = [:attrasgn, [:vcall, :a], :b, [:array, [:fixnum, 8]]]
    
    gen x do |g|
      g.push 8
      g.push :self
      g.send :a, 0, true
      g.send :b=, 1, false
    end
  end
  
  it "compiles 'self[index, 0] = other_string'" do
    x = [:attrasgn, nil, :[]=, 
      [:array, [:vcall, :index], [:fixnum, 0], 
        [:vcall, :other_string]
      ]
    ]
    
    gen x do |g|
      g.push :self
      g.send :other_string, 0, true
      g.push 0
      g.push :self
      g.send :index, 0, true
      g.push :self
      g.send :[]=, 3, false
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
      g.get_args
      g.push 2
      g.swap
      g.push 1
      g.swap
      
      g.push :nil
      g.swap
      g.push :self
      g.swap
      g.set_args
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
      g.get_args
      g.push :nil
      g.swap
      g.push :self
      g.send :f, 0, true
      g.swap
      g.set_args
      g.send_with_register :[]=, false
    end
  end
  
  it "compiles 'undef :blah'" do
    gen [:undef, :blah] do |g|
      g.push_literal :blah
      g.push :self
      g.send :metaclass, 0
      g.send :undef_method, 1
    end
  end

  it "compiles 'class B; undef :blah; end'" do
    x = [:class, [:colon2, :A], nil, [:scope, [:undef, :blah], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push_literal :blah
        d.push :self
        d.send :undef_method, 1
        d.sret
      end
      
      g.push :nil
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0
      g.push_encloser
      
    end
  end
  
  it "compiles 'yield'" do
    x = [:yield, nil, false]
    
    gen x do |g|
      g.push_block
      g.send :call, 0
    end
  end
  
  it "compiles 'yield 1'" do
    x = [:yield, [:fixnum, 1], false]
    
    gen x do |g|
      g.push 1
      g.push_block
      g.send :call, 1
    end
  end
  
  it "compiles 'yield 1, 2'" do
    x = [:yield, [:array, [:fixnum, 1], [:fixnum, 2]], true]
    
    gen x do |g|
      g.push 2
      g.push 1
      g.push_block
      g.send :call, 2
    end
  end
  
  it "compiles 'yield [1, 2]'" do
    x = [:yield, [:array, [:fixnum, 1], [:fixnum, 2]], false]
    
    gen x do |g|
      g.push 1
      g.push 2
      g.make_array 2
      g.push_block
      g.send :call, 1
    end
  end
  
  it "compiles 'yield *a'" do
    x = [:yield, [:splat, [:vcall, :a]], false]
    
    gen x do |g|
      g.push :self
      g.send :a, 0, true
      g.cast_array_for_args 0
      g.push_array
      g.get_args
      
      g.set_args
      g.push_block
      g.send_with_register :call, false
    end
  end
  
  
  it "compiles 'super(1)'" do
    x = [:defn, :a, [:scope, [:block, [:args],
          [:super, [:array, [:fixnum, 1]]]], []]]
    
    gen x do |g|
      desc = description do |d|
        d.check_argcount 0, 0
        d.push 1
        d.push :nil
        d.super :a, 1
        d.sret
      end
      
      g.push_literal desc
      g.push :self
      g.add_method :a
    end
  end
  
  it "compiles 'super(*blah)'" do
    x = [:defn, :a, [:scope, [:block, [:args],
          [:super, [:splat, [:vcall, :blah]]]], []]]
    
    gen x do |g|
      desc = description do |d|
        d.check_argcount 0, 0
        d.push :self
        d.send :blah, 0, true
        d.cast_array_for_args 0
        d.push_array
        d.get_args
        d.push :nil
        d.swap
        d.set_args
        d.super_with_register :a
        d.sret
      end
      
      g.push_literal desc
      g.push :self
      g.add_method :a
    end
  end
  
  
  it "compiles 'super'" do
    x = [:defn, :a, [:scope, [:block, [:args, [:a, :b], [], nil, nil],
          [:zsuper]], []]]
    
    gen x do |g|
      desc = description do |d|
        d.check_argcount 2, 2
        d.from_fp 1
        d.from_fp 0
        d.push :nil        
        d.super :a, 2
        d.sret
      end
      
      g.push_literal desc
      g.push :self
      g.add_method :a
      
    end
        
  end
  
  it "compiles 'super' with a splat in the arg list" do
    x = [:defn, :a, [:scope, [:block, [:args, [:name], [], [:rest, 1], nil],
          [:zsuper]], []]]
    
    gen x do |g|
      desc = description do |d|
        d.check_argcount 1, 1024
        d.make_rest_fp 1
        d.set_local 0
        d.pop
        d.push_local 0
        d.cast_array_for_args 1
        d.push_array
        d.get_args
        d.from_fp 0
        d.swap        
        d.push :nil
        d.swap
        d.set_args
        d.super_with_register :a
        d.sret
      end
      
      g.push_literal desc
      g.push :self
      g.add_method :a
      
    end
        
  end
  
  it "compiles 'foo(&blah)'" do
    gen [:block_pass, [:vcall, :blah], [:fcall, :foo]] do |g|
      g.push :self
      g.send :blah, 0, true
      g.push :self
      g.send_with_block :foo, 0, true 
    end
  end
end
