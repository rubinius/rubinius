require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#each" do
  it "splits self using the supplied record separator and passes each substring to the block" do
    a = []
    "one\ntwo\r\nthree".each("\n") { |s| a << s }
    a.should == ["one\n", "two\r\n", "three"]
    
    b = []
    "hello\nworld".each('l') { |s| b << s }
    b.should == [ "hel", "l", "o\nworl", "d" ]
    
    c = []
    "hello\n\n\nworld".each("\n") { |s| c << s }
    c.should == ["hello\n", "\n", "\n", "world"]
  end
  
  it "taints substrings that are passed to the block if self is tainted" do
    "one\ntwo\r\nthree".taint.each { |s| s.tainted?.should == true }

    "x.y.".each(".".taint) { |s| s.tainted?.should == false }
  end
  
  it "passes self as a whole to the block if the separator is nil" do
    a = []
    "one\ntwo\r\nthree".each(nil) { |s| a << s }
    a.should == ["one\ntwo\r\nthree"]
  end
  
  it "appends multiple successive newlines together when the separator is an empty string" do
    a = []
    "hello\nworld\n\n\nand\nuniverse\n\n\n\n\n".each('') { |s| a << s }
    a.should == ["hello\nworld\n\n\n", "and\nuniverse\n\n\n\n\n"]
  end

  it "uses $/ as the separator when none is given" do
    [
      "", "x", "x\ny", "x\ry", "x\r\ny", "x\n\r\r\ny",
      "hello hullo bello"
    ].each do |str|
      ["", "llo", "\n", "\r", nil].each do |sep|
        begin
          expected = []
          str.each(sep) { |x| expected << x }

          old_rec_sep, $/ = $/, sep

          actual = []
          str.each { |x| actual << x }

          actual.should == expected
        ensure
          $/ = old_rec_sep
        end
      end
    end
  end
  
  it "yields subclass instances for subclasses" do
    a = []
    MyString.new("hello\nworld").each { |s| a << s.class }
    a.should == [MyString, MyString]
  end
  
  it "returns self" do
    s = "hello\nworld"
    (s.each {}).equal?(s).should == true
  end

  it "tries to convert the separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() 'l' end
    
    a = []
    "hello\nworld".each(separator) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "l")
    
    a = []
    "hello\nworld".each(obj) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
  end
  
  it "raises a RuntimeError if the string is modified while substituting" do
    str = "hello\nworld"
    should_raise(RuntimeError) { str.each_line { str[0] = 'x' } }
  end
  
  it "raises a TypeError when the separator can't be converted to a string" do
    should_raise(TypeError) { "hello world".each(?o) }
    should_raise(TypeError) { "hello world".each(:o) }
    should_raise(TypeError) { "hello world".each(Object.new) }
  end
end
