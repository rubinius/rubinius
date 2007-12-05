require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  it "compiles a defn with no args" do
    x = [:defn, :a, [:scope, [:block, [:args],
           [:fixnum, 12]], []]]
           
    gen x do |g|
      meth = description do |d|
        d.check_argcount 0, 0
        d.push 12
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :a
    end
  end
  
  it "compiles 'def add(a,b); a + b; end'" do
    x = [:defn, :add, 
          [:scope, 
            [:block, 
              [:args, [:a, :b], [], nil, nil], 
              [:call, [:lvar, :a, 0], :+, [:array, [:lvar, :b, 0]]]
            ], 
            [:a, :b]
          ]
        ]
    
    gen x do |g|
      meth = description do |d|
        d.check_argcount 2, 2
        d.from_fp 1
        d.from_fp 0
        d.send :+, 1, false
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :add
    end
  end
  
  it "compiles 'def add(a,b=2); a + b; end'" do
    x = [:defn, :add, 
          [:scope, 
            [:block, 
              [:args, [:a], [:b], nil, 
                [:block, [:lasgn, :b, 0, [:lit, 2]]]], 
              [:call, [:lvar, :a, 0], :+, [:array, [:lvar, :b, 0]]]
            ], 
            [:a, :b]
          ]
        ]
    
    gen x do |g|
      meth = description do |d|
        d.check_argcount 1, 2
        up = d.new_label
        dn = d.new_label
        d.passed_arg 1
        d.git up
        d.push 2
        d.set_local 0
        d.pop
        d.goto dn
        
        up.set!
        d.set_local_from_fp 0, 1
        dn.set!
        
        d.push_local 0
        d.from_fp 0
        d.send :+, 1, false
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :add
    end
  end
  
  it "compiles 'def add(a); [].each { |b| a + b }; end'" do
    x = [:defn, :add, 
          [:scope, 
            [:block, 
              [:args, [:a], [], nil, nil],
              [:iter, [:call, [:zarray], :each], 
                [:lasgn, :b, 0], 
                [:block, [:dasgn_curr, :b], 
                  [:call, [:lvar, :a, 0], :+, [:array, [:lvar, :b, 0]]]
                ]
              ]
            ], 
            [:a, :b]
          ]
        ]
    
    gen x do |g|
      meth = description do |d|
        d.check_argcount 1, 1
        d.set_local_from_fp 0, 0

        iter = description do |i|
          i.cast_for_single_block_arg
          i.set_local_depth 0, 0
          i.pop

          i.new_label.set! # redo
          
          i.push_local_depth 0, 0
          i.push_local 0
          i.send :+, 1, false
          i.soft_return
        end

        d.push_literal iter
        d.create_block2
        d.make_array 0
        d.send_with_block :each, 0, false
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :add
    end
  end
  
  it "compiles 'def a(*b); nil; end'" do
    x = [:defn, :a, 
      [:scope, 
        [:block, [:args, [], [], [:b, 1], nil], 
          [:lvar, :b, 0]
        ], 
        [:b]
      ]
    ]
    
    gen x do |g|
      meth = description do |d|
        d.check_argcount 0, 1024
        d.make_rest_fp 0
        d.set_local 0
        d.pop
        d.push_local 0
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :a
    end
  end
  
  it "compiles 'def a(&b); b; end'" do
    x = [:defn, :a, 
      [:scope, 
        [:block, [:args], [:block_arg, :b, 0], [:lvar, :b, 0]], 
        [:b]
      ]
    ]
    
    gen x do |g|
      meth = description do |d|
        d.check_argcount 0, 0
        d.push_block
        d.push_const :Proc
        d.send :from_environment, 1
        d.set_local 0
        d.pop
        d.push_local 0
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :a
    end
  end
  
  it "compiles a defs" do
    x = [:defs, [:vcall, :a], :go, [:scope, [:block, [:args], 
          [:fixnum, 12]], []]]
          
    gen x do |g|
      meth = description do |d|
        d.check_argcount 0, 0
        d.push 12
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.send :a, 0, true
      g.attach_method :go
    end
  end

  it "compiles 'lambda { def a(x); x; end }'" do
    x = [:iter, [:fcall, :lambda], nil,
          [:defn, :a,
            [:scope,
              [:block, [:args, [:x], [], nil, nil], [:lvar, :x, 0]],
              [:x]
            ]
          ]
        ]
    gen x do |g|
      lam = description do |l|
        meth = description do |m|
          m.check_argcount 1, 1
          m.from_fp 0
          m.sret
        end

        l.pop
        l.new_label.set!
        l.push_literal meth
        l.push :self
        l.add_method :a
        l.soft_return
      end

      g.push_literal lam
      g.create_block2
      g.push :self
      g.send_with_block :lambda, 0, true
    end
  end
  
  it "compiles 'class << x; 12; end'" do
    x = [:sclass, [:vcall, :x], [:scope, [:lit, 12], []]]
    
    gen x do |g|
      meth = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
        d.sret
      end
      
      g.push :self
      g.send :x, 0, true
      g.dup
      g.send :__verify_metaclass__, 0
      g.pop
      g.open_metaclass
      g.dup
      g.push_literal meth
      g.swap
      g.attach_method :__metaclass_init__
      g.pop
      g.send :__metaclass_init__, 0
      g.push_encloser
    end
  end
  
  it "compiles a class with no superclass" do
    x = [:class, [:colon2, :A], nil, [:scope, [:lit, 12], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
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
  
  it "compiles a class declared at a path" do
    x = [:class, [:colon2, [:const, :B], :A], nil, [:scope, [:lit, 12], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
        d.sret
      end
      
      g.push_const :B
      g.push :nil
      g.open_class_under :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__, 0
      g.push_encloser
    end
  end
  
  it "compiles a class with superclass" do
    x = [:class, [:colon2, :A], [:const, :B], [:scope, [:lit, 12], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
        d.sret
      end
      
      g.push_const :B
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
  
  it "compiles a class with space allocated for locals" do
    x = [:class, [:colon2, :A], nil,
          [:block, [:lasgn, :a, 0, [:fixnum, 1]]]]
          
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 1
        d.set_local 0
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
  
  it "compiles a normal module" do
    x = [:module, [:colon2, :A], [:scope, [:lit, 12], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
        d.sret
      end
      
      g.open_module :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__module_init__
      g.pop
      g.send :__module_init__, 0
      g.push_encloser
    end
  end
  
  it "compiles a module declared at a path" do
    x = [:module, [:colon2, [:const, :B], :A], [:scope, [:lit, 12], []]]
    
    gen x do |g|
      desc = description do |d|
        d.push :self
        d.set_encloser
        d.push 12
        d.sret
      end
      
      g.push_const :B
      g.open_module_under :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__module_init__
      g.pop
      g.send :__module_init__, 0
      g.push_encloser
    end
  end
  
  
end
