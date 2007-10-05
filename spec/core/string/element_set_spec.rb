require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

# TODO: Add missing String#[]= specs:
#   String#[range] = obj
#   String#[re] = obj
#   String#[re, idx] = obj
#   String#[str] = obj

describe "String#[]= with index" do
  it "sets the code of the character at idx to char modulo 256" do
    a = "hello"
    a[0] = ?b
    a.should == "bello"
    a[-1] = ?a
    a.should == "bella"
    a[-1] = 0
    a.should == "bell\x00"
    a[-5] = 0
    a.should == "\x00ell\x00"
    
    a = "x"
    a[0] = ?y
    a.should == "y"
    a[-1] = ?z
    a.should == "z"
    
    a[0] = 255
    a[0].should == 255
    a[0] = 256
    a[0].should == 0
    a[0] = 256 * 3 + 42
    a[0].should == 42
    a[0] = -214
    a[0].should == 42
  end
 
  it "raises an IndexError without changing self if idx is outside of self" do
    a = "hello"
    
    should_raise(IndexError) { a[20] = ?a }
    a.should == "hello"
    
    should_raise(IndexError) { a[-20] = ?a }
    a.should == "hello"
    
    should_raise(IndexError) { ""[0] = ?a }
    should_raise(IndexError) { ""[-1] = ?a }
  end

  version '1.8.4'..'1.8.5' do
    it "raises IndexError when passed other than a Fixnum" do
      str = "hello"
      should_raise(IndexError) { str[0.5] = ?c }
    
      obj = Object.new
      obj.should_receive(:to_int, :returning => -1)
      should_raise(IndexError) { str[obj] = ?y }
    
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
      should_raise(IndexError) { str[obj] = ?! }
    end
  end
  
  version '1.8.6' do
    it "calls to_int on index" do
      str = "hello"
      str[0.5] = ?c
      str.should == "cello"
    
      obj = Object.new
      obj.should_receive(:to_int, :returning => -1)
      str[obj] = ?y
      str.should == "celly"
    
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
      str[obj] = ?!
      str.should == "cell!"
    end
  end
  
  it "sets the code to char % 256" do
    str = "Hello"
    
    str[0] = ?a + 256 * 3
    str[0].should == ?a
    str[0] = -200
    str[0].should == 56
  end
  
  it "doesn't call to_int on char" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    should_raise(TypeError) { "hi"[0] = obj }
  end
  
  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) { a[0] = ?b }
  end
end

describe "String#[]= with String" do
  it "replaces the char at idx with other_str" do
    a = "hello"
    a[0] = "bam"
    a.should == "bamello"
    a[-2] = ""
    a.should == "bamelo"
  end

  it "taints self if other_str is tainted" do
    a = "hello"
    a[0] = "".taint
    a.tainted?.should == true
    
    a = "hello"
    a[0] = "x".taint
    a.tainted?.should == true
  end

  it "raises an IndexError without changing self if idx is outside of self" do
    str = "hello"

    should_raise(IndexError) { str[20] = "bam" }    
    str.should == "hello"
    
    should_raise(IndexError) { str[-20] = "bam" }
    str.should == "hello"

    should_raise(IndexError) { ""[0] = "bam" }
    should_raise(IndexError) { ""[-1] = "bam" }
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) { a[0] = "bam" }
  end

  version '1.8.4'..'1.8.5' do
    it "raises IndexError when passed other than a Fixnum" do
      str = "hello"
      should_raise(IndexError) { str[0.5] = "hi " }
    
      obj = Object.new
      obj.should_receive(:to_int, :returning => -1)
      should_raise(IndexError) { str[obj] = "!" }
    
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
      should_raise(IndexError) { str[obj] = "e vator" }
    end
  end
  
  version '1.8.6' do
    it "calls to_int on index" do
      str = "hello"
      str[0.5] = "hi "
      str.should == "hi ello"
    
      obj = Object.new
      obj.should_receive(:to_int, :returning => -1)
      str[obj] = "!"
      str.should == "hi ell!"
    
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
      str[obj] = "e vator"
      str.should == "hi elle vator"
    end
  end
  
  it "tries to convert other_str to a String using to_str" do
    other_str = Object.new
    def other_str.to_str() "-test-" end
    
    a = "abc"
    a[1] = other_str
    a.should == "a-test-c"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "ROAR")

    a = "abc"
    a[1] = obj
    a.should == "aROARc"
  end
  
  it "raises a TypeError if other_str can't be converted to a String" do
    should_raise(TypeError) { "test"[1] = :test }
    should_raise(TypeError) { "test"[1] = Object.new }
    should_raise(TypeError) { "test"[1] = nil }
  end
end

describe "String#[]= with index, count" do
  it "starts at idx and overwrites count characters before inserting the rest of other_str" do
    a = "hello"
    a[0, 2] = "xx"
    a.should == "xxllo"
    a = "hello"
    a[0, 2] = "jello"
    a.should == "jellollo"
  end
 
  it "counts negative idx values from end of the string" do
    a = "hello"
    a[-1, 0] = "bob"
    a.should == "hellbobo"
    a = "hello"
    a[-5, 0] = "bob"
    a.should == "bobhello"
  end
 
  it "overwrites and deletes characters if count is more than the length of other_str" do
    a = "hello"
    a[0, 4] = "x"
    a.should == "xo"
    a = "hello"
    a[0, 5] = "x"
    a.should == "x"
  end
 
  it "deletes characters if other_str is an empty string" do
    a = "hello"
    a[0, 2] = ""
    a.should == "llo"
  end
 
  it "deletes characters up to the maximum length of the existing string" do
    a = "hello"
    a[0, 6] = "x"
    a.should == "x"
    a = "hello"
    a[0, 100] = ""
    a.should == ""
  end
 
  it "appends other_str to the end of the string if idx == the length of the string" do
    a = "hello"
    a[5, 0] = "bob"
    a.should == "hellobob"
  end
  
  it "taints self if other_str is tainted" do
    a = "hello"
    a[0, 0] = "".taint
    a.tainted?.should == true
    
    a = "hello"
    a[1, 4] = "x".taint
    a.tainted?.should == true
  end
 
  it "raises an IndexError if |idx| is greater than the length of the string" do
    should_raise(IndexError) { "hello"[6, 0] = "bob" }
    should_raise(IndexError) { "hello"[-6, 0] = "bob" }
  end
 
  it "raises an IndexError if count < 0" do
    should_raise(IndexError) { "hello"[0, -1] = "bob" }
    should_raise(IndexError) { "hello"[1, -1] = "bob" }
  end
 
  it "raises a TypeError if other_str is a type other than String" do
    should_raise(TypeError) { "hello"[0, 2] = nil }
    should_raise(TypeError) { "hello"[0, 2] = :bob }
    should_raise(TypeError) { "hello"[0, 2] = 33 }
  end
end
