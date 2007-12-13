require File.dirname(__FILE__) + "/helper.rb"

describe Compiler2 do
  it "compiles true" do
    gen [:true] do |g|
      g.push :true
    end
  end
  
  it "compiles false" do
    gen [:false] do |g|
      g.push :false
    end
  end
  
  it "compiles nil" do
    gen [:nil] do |g|
      g.push :nil
    end
  end
  
  it "compiles self" do
    gen [:self] do |g|
      g.push :self
    end
  end
  
  it "compiles and" do
    gen [:and, [:true], [:false]] do |g|
      g.push :true
      g.dup
      lbl = g.new_label
      g.gif lbl
      g.pop
      g.push :false
      lbl.set!
    end
  end
  
  it "compiles or" do
    gen [:or, [:true], [:false]] do |g|
      g.push :true
      g.dup
      lbl = g.new_label
      g.git lbl
      g.pop
      g.push :false
      lbl.set!
    end
  end
  
  it "compiles not" do
    gen [:not, [:true]] do |g|
      g.push :true
      tru = g.new_label
      fin = g.new_label
      g.git tru
      g.push :true
      g.goto fin
      tru.set!
      g.push :false
      fin.set!
    end
  end
  
  it "compiles numbers" do
    gen [:fixnum, 3] do |g|
      g.push 3
    end
  end
  
  it "compiles symbols" do
    gen [:lit, :blah] do |g|
      g.push_literal :blah
    end
  end
  
  it "compiles numbers as literals" do
    gen [:lit, 3] do |g|
      g.push 3
    end
  end
  
  it "compiles bignums" do
    num = 10 ** 10
    gen [:lit, num] do |g|
      g.push_literal num
    end
  end
  
  it "compiles floats" do
    num = 1.2
    gen [:lit, num] do |g|
      g.push_literal num
    end
  end
  
  it "compiles strings" do
    gen [:lit, "blah"] do |g|
      g.push_literal "blah"
    end
  end
  
  it "compiles negate with fixnum" do
    gen [:negate, [:fixnum, 10]] do |g|
      g.push -10
    end
  end
  
  it "compiles negate with anything else" do
    gen [:negate, [:str, "str"]] do |g|
      g.push_literal "str"
      g.string_dup
      g.send :"@-", 0
    end
  end
  
  it "compiles regexp literals" do
    re = /abcd/
    gen [:lit, re] do |g|
      idx = g.add_literal(nil)
      g.push_literal_at idx
      g.dup
      g.is_nil
      
      lbl = g.new_label
      g.gif lbl
      g.pop
      
      g.push re.options
      g.push_literal "abcd"
      g.push_const :Regexp
      g.send :new, 2
      g.set_literal idx
      lbl.set!
    end
  end
  
  it "compiles regexs" do
    gen [:regex, "comp", 0] do |g|
      idx = g.add_literal(nil)
      g.push_literal_at idx
      g.dup
      g.is_nil
      
      lbl = g.new_label
      g.gif lbl
      g.pop
      
      g.push 0
      g.push_literal "comp"
      g.push_const :Regexp
      g.send :new, 2
      g.set_literal idx
      lbl.set!
    end
  end
  
  it "compiles dynamic strings" do
    gen [:dstr, "hello ", [:evstr, [:true]], [:str, "!"]] do |g|
      g.push_literal "!"
      g.string_dup
      g.push :true
      g.send :to_s, 0, true
      g.push_literal "hello "
      g.string_dup
      g.string_append
      g.string_append
    end
  end
  
  it "compiles empty dynamic strings" do
    gen [:dstr, "hello ", [:evstr]] do |g|
      g.push_literal ""
      g.string_dup
      g.send :to_s, 0, true
      g.push_literal "hello "
      g.string_dup
      g.string_append
    end
  end
  
  it "compiles dynamic regexs" do
    gen [:dregx, "(", [:evstr, [:true]], [:str, ")"]] do |g|
      g.push_literal ")"
      g.string_dup
      g.push :true
      g.send :to_s, 0, true
      g.push_literal "("
      g.string_dup
      g.string_append
      g.string_append
      g.push_const :Regexp
      g.send :new, 1
    end
  end
  
  it "compiles empty dynamic regexs" do
    gen [:dregx, "(", [:evstr], [:str, ")"]] do |g|
      g.push_literal ")"
      g.string_dup
      g.push_literal ""
      g.string_dup
      g.send :to_s, 0, true
      g.push_literal "("
      g.string_dup
      g.string_append
      g.string_append
      g.push_const :Regexp
      g.send :new, 1
    end
  end
  
  it "compiles a dynamic regex once is indicated" do
    gen [:dregx_once, "(", [:evstr, [:true]], [:str, ")"]] do |g|
      idx = g.push_literal nil
      g.dup
      g.is_nil
      
      lbl = g.new_label
      g.gif lbl
      g.pop
      
      g.push_literal ")"
      g.string_dup
      g.push :true
      g.send :to_s, 0, true
      g.push_literal "("
      g.string_dup
      g.string_append
      g.string_append
      g.push_const :Regexp
      g.send :new, 1
      
      g.set_literal idx
      lbl.set!
    end
  end
  
  it "compiles regexp operation =~" do
    gen [:match2, [:regex, "aoeu", 0], [:str, "blah"]] do |g|
      g.push_literal "blah"
      g.string_dup
      
      idx = g.add_literal(nil)
      g.push_literal_at idx
      g.dup
      g.is_nil
      
      lbl = g.new_label
      g.gif lbl
      g.pop
      
      g.push 0
      g.push_literal "aoeu"
      g.push_const :Regexp
      g.send :new, 2
      g.set_literal idx
      lbl.set!
      
      g.send :=~, 1
    end
  end
  
  it "compiles string operation =~" do
    gen [:match3, [:regex, "aoeu", 0], [:str, "blah"]] do |g|
      idx = g.add_literal(nil)
      g.push_literal_at idx
      g.dup
      g.is_nil
      
      lbl = g.new_label
      g.gif lbl
      g.pop
      
      g.push 0
      g.push_literal "aoeu"
      g.push_const :Regexp
      g.send :new, 2
      g.set_literal idx
      lbl.set!
      
      g.push_literal "blah"
      g.string_dup
      
      g.send :=~, 1
    end
  end
  
  it "compiles regexp back references" do
    gen [:back_ref, 38] do |g|
      g.push_literal :&
      g.push_context
      g.send :back_ref, 1
    end
  end
  
  it "compiles regexp last match captures" do
    gen [:nth_ref, 3] do |g|
      g.push 3
      g.push_context
      g.send :nth_ref, 1
    end
  end
  
  it "compiles a literal array" do
    gen [:array, [:fixnum, 12], [:fixnum, 13]] do |g|
      g.push 12
      g.push 13
      g.make_array 2 
    end
  end
  
  it "compiles an empty array" do
    gen [:zarray] do |g|
      g.make_array 0
    end
  end
  
  it "compiles a hash literal" do
    gen [:hash, [:fixnum, 12], [:fixnum, 13], [:fixnum, 14], [:fixnum, 15]] do |g|
      g.push 15
      g.push 14
      g.push 13
      g.push 12
      g.push_cpath_top
      g.find_const :Hash
      g.send :[], 4
    end
  end
  
  it "compiles an svalue" do
    gen [:svalue, [:vcall, :ab]] do |g|
      g.push :self
      g.send :ab, 0, true
      g.cast_array
      g.dup
      g.send :size, 0
      g.push 1
      g.send :>, 1
      
      lbl = g.new_label
      g.git lbl
      
      g.push 0
      g.swap
      g.send :at, 1
      
      lbl.set!
    end
  end
  
  it "compiles a splat" do
    gen [:splat, [:fixnum, 12]] do |g|
      g.push 12
      g.cast_array
    end
  end
  
  it "compiles :to_ary" do
    gen [:to_ary, [:fixnum, 12]] do |g|
      g.push 12
      g.cast_array
    end
  end
  
  it "compiles '`ls`'" do
    gen [:xstr, "ls"] do |g|
      g.push_literal "ls"
      g.string_dup
      g.push :self
      g.send :`, 1
    end
  end
  
  it "compiles '`ls \#{dir}`'" do
    gen [:dxstr, "ls ", [:evstr, [:vcall, :dir]]] do |g|
      g.push :self
      g.send :dir, 0, true
      g.send :to_s, 0, true
      g.push_literal "ls "
      g.string_dup
      g.string_append
      g.push :self
      g.send :`, 1, true
    end
  end
  
  it "compiles ':\"you \#{thing}\"'" do
    gen [:dsym, [:dstr, "you ", [:evstr, [:vcall, :thing]]]] do |g|
      g.push :self
      g.send :thing, 0, true
      g.send :to_s, 0, true
      g.push_literal "you "
      g.string_dup
      g.string_append
      g.send :to_sym, 0, true
    end
    
  end
  
  it "compiles 'alias a b'" do
    gen [:alias, :b, :a] do |g|
      g.push_literal :b
      g.push_literal :a
      g.push :self
      g.send :alias_method, 2
    end
  end
  
  it "compiles '1..2'" do
    gen [:dot2, [:fixnum, 1], [:fixnum, 2]] do |g|
      g.push 2
      g.push 1
      g.push_cpath_top
      g.find_const :Range
      g.send :new, 2 
    end
  end
  
  it "compiles '1...2'" do
    gen [:dot3, [:fixnum, 1], [:fixnum, 2]] do |g|
      g.push :true
      g.push 2
      g.push 1
      g.push_cpath_top
      g.find_const :Range
      g.send :new, 3
    end
  end
  
end
