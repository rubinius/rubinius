require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  it "compiles a defn with no args" do
    x = [:defn, :a, [:scope, [:block, [:args],
           [:fixnum, 12]], []]]
           
    gen x do |g|
      meth = description do |d|
        d.push 12
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :a
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
      g.send :__metaclass_init__
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
      g.send :__class_init__
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
      g.send :__class_init__
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
      g.send :__class_init__
      g.push_encloser
    end
  end
  
  it "compiles a class with space allocated for locals" do
    x = [:class, [:colon2, :A], nil,
          [:block, [:lasgn, :a, 0, [:fixnum, 1]]]]
          
    gen x do |g|
      desc = description do |d|
        d.allocate_stack 1
        d.push :self
        d.set_encloser
        d.push 1
        d.set_local_fp 1
        d.sret
      end
      
      g.push :nil
      g.open_class :A
      g.dup
      g.push_literal desc
      g.swap
      g.attach_method :__class_init__
      g.pop
      g.send :__class_init__
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
      g.send :__module_init__
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
      g.send :__module_init__
      g.push_encloser
    end
  end
  
  
end
