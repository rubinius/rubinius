require File.dirname(__FILE__) + "/helper.rb"

describe Compiler do
  it "compiles simple lasgn / lvar" do
    x = [:block, [:lasgn, :a, 0, [:fixnum, 12]], [:lvar, :a, 0]]
    
    gen x do |g|
      g.push 12
      g.set_local 0
      g.pop
      g.push_local 0
    end
  end
  
  it "compiles lvars only after declared" do
    x =   [:block, [:vcall, :name],
                   [:lasgn, :name, 0, [:lit, 3]],
                   [:lvar, :name, 0]]
    gen x do |g|
      g.push :self
      g.send :name, 0, true
      g.pop
      g.push 3
      g.set_local 0
      g.pop
      g.push_local 0
    end
    
  end
  
  it "compiles lvar for argument" do
    x = [:defn, :meth, [:scope, [:block, [:args, [:b], [], nil, nil],
           [:lvar, :b, 0]], []]]
           
    gen x do |g|
      meth = description do |d|
        d.check_argcount 1, 1
        d.from_fp 0
        d.sret
      end
      
      g.push_literal meth
      g.push :self
      g.add_method :meth
    end
  end
  
  it "compiles toplevel lvar into locals when closure referenced" do
    x = [:block, 
          [:lasgn, :name, 0, [:fixnum, 12]],
          [:iter, [:fcall, :go], nil, [:lvar, :name, 0]]]
            
    gen x do |g|
      iter = description do |d|
        d.pop
        d.new_label.set! # redo
        d.push_local 0
        d.soft_return
      end

      g.push 12
      g.set_local 0
      g.pop
      g.push_literal iter
      g.create_block2
      g.push :self
      g.send_with_block :go, 0, true
    end          
  end
  
  it "compiles block lvar into depth referenced lvar" do
    ax = [:iter, [:fcall, :go], nil, [:block, 
            [:lasgn, :name, 0, [:fixnum, 12]],
            [:lvar, :name, 0]]]
    
    gen ax do |g|
      iter = description do |d|
        d.pop
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop
        d.push_local_depth 0, 0
        d.soft_return
      end

      g.push_literal iter
      g.create_block2
      g.push :self
      g.send_with_block :go, 0, true
    end          
  end
  
  it "compiles block lvar into depth referenced lvar upward" do
    ax = [:iter, [:fcall, :go], nil, [:block, 
            [:lasgn, :name, 0, [:fixnum, 12]],
            [:iter, [:fcall, :go], nil, [:block,
              [:lvar, :name, 0]        
            ]]
         ]]
    
    gen ax do |g|
      iter = description do |d|
        d.pop
        d.new_label.set! # redo
        d.push 12
        d.set_local_depth 0, 0
        d.pop
        i2 = description do |j|
          j.pop
          j.new_label.set! # redo
          j.push_local_depth 1, 0
          j.soft_return
        end
        
        d.push_literal i2
        d.create_block2
        d.push :self
        d.send_with_block :go, 0, true
        
        d.soft_return
      end

      g.push_literal iter
      g.create_block2
      g.push :self
      g.send_with_block :go, 0, true
    end          
  end
  
  # Ivars and slots
  it "compiles '@blah'" do
    gen [:ivar, :@blah] do |g|
      g.push_ivar :@blah
    end
  end
  
  it "compiles '@blah = 1'" do
    gen [:iasgn, :@blah, [:lit, 1]] do |g|
      g.push 1
      g.set_ivar :@blah
    end
  end
  
  it "compiles '@blah' when blah is a slot" do
    x = [:block,
      [:fcall, :ivar_as_index, [:array, [:ihash, [:lit, :blah], [:fixnum, 12]]]],
      [:ivar, :@blah]]
    
    gen x do |g|
      g.push_my_field 12
    end
  end
  
  it "compiles '@blah = 1' when blah is a slot" do
    x = [:block,
      [:fcall, :ivar_as_index, [:array, [:ihash, [:lit, :blah], [:fixnum, 12]]]],
      [:iasgn, :@blah, [:fixnum, 9]]]
    
    gen x do |g|
      g.push 9
      g.store_my_field 12
    end
  end
  
  it "compiles '$var'" do
    gen [:gvar, :$var] do |g|
      g.push_literal :$var
      g.push_cpath_top
      g.find_const :Globals
      g.send :[], 1
    end
  end
  
  it "compiles '$var = 1'" do
    gen [:gasgn, :$var, [:fixnum, 12]] do |g|
      g.push 12
      g.push_literal :$var
      g.push_cpath_top
      g.find_const :Globals
      g.send :[]=, 2
    end    
  end
  
  it "compiles '$!'" do
    gen [:gvar, :$!] do |g|
      g.push_exception
    end
  end
  
  it "compiles '$! = 1'" do
    gen [:gasgn, :$!, [:fixnum, 1]] do |g|
      g.push 1
      g.raise_exc
    end
  end
  
  it "compiles 'String'" do
    gen [:const, :String] do |g|
      g.push_const :String
    end
  end
  
  it "compiles 'Object::String'" do
    gen [:colon2, [:const, :Object], :String] do |g|
      g.push_const :Object
      g.find_const :String
    end
  end
  
  it "compiles '::String'" do
    gen [:colon3, :String] do |g|
      g.push_cpath_top
      g.find_const :String
    end
  end
  
  it "compiles 'Blah = 1'" do
    gen [:cdecl, :Blah, [:lit, 1], nil] do |g|
      g.push 1
      g.set_const :Blah
    end
  end
  
  it "compiles 'Object::Blah = 1'" do
    gen [:cdecl, nil, [:lit, 1], [:colon2, [:const, :Object], :Blah]] do |g|
      g.push 1
      g.push_const :Object
      g.set_const :Blah, true
    end
  end
  
  it "compiles '::Blah = 1'" do
    gen [:cdecl, nil, [:lit, 1], [:colon3, :Blah]] do |g|
      g.push 1
      g.push_cpath_top
      g.set_const :Blah, true
    end
  end
end
