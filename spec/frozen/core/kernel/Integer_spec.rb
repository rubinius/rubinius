require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require "rational"

describe :kernel_integer, :shared => true do
  it "returns a Bignum for a Bignum" do
    Integer(2e100).should == 2e100
  end
  
  it "returns a Fixnum for a Fixnum" do
    Integer(100).should == 100
  end

  it "uncritically return the value of to_int even if it is not an Integer" do
    obj = mock("object")
    obj.should_receive(:to_int).and_return("1")
    obj.should_not_receive(:to_i)
    Integer(obj).should == "1"
  end
  
  ruby_version_is "1.9" do
    it "raises a TypeError when passed nil" do
      lambda { Integer(nil) }.should raise_error(TypeError)
    end
  end
  
  ruby_version_is ""..."1.9" do
    it "returns 0 when passed nil" do
      Integer(nil).should == 0
    end
  end

  it "returns a Fixnum or Bignum object" do
    Integer(2).should be_an_instance_of(Fixnum)
    Integer(9**99).should be_an_instance_of(Bignum)
  end
  
  it "truncates Floats" do
    Integer(3.14).should == 3
    Integer(90.8).should == 90
  end
    
  it "calls to_i on Rationals" do
    Integer(Rational(8,3)).should == 2
    Integer(3.quo(2)).should == 1
  end

  it "returns the value of to_int if the result is a Fixnum" do
    obj = mock("object")
    obj.should_receive(:to_int).and_return(1)
    obj.should_not_receive(:to_i)
    Integer(obj).should == 1
  end

  it "returns the value of to_int if the result is a Bignum" do
    obj = mock("object")
    obj.should_receive(:to_int).and_return(2e100)
    obj.should_not_receive(:to_i)
    Integer(obj).should == 2e100
  end

  it "calls to_i on an object whose to_int returns nil" do
    obj = mock("object")
    obj.should_receive(:to_int).and_return(nil)
    obj.should_receive(:to_i).and_return(1)
    Integer(obj).should == 1
  end

  it "uncritically return the value of to_int even if it is not an Integer" do
    obj = mock("object")
    obj.should_receive(:to_int).and_return("1")
    obj.should_not_receive(:to_i)
    Integer(obj).should == "1"
  end

  it "raises a TypeError if to_i returns a value that is not an Integer" do
    obj = mock("object")
    obj.should_receive(:to_i).and_return("1")
    lambda { Integer(obj) }.should raise_error(TypeError)
  end

  it "raises a TypeError if no to_int or to_i methods exist" do
    obj = mock("object")
    lambda { Integer(obj) }.should raise_error(TypeError)
  end

  it "raises a TypeError if to_int returns nil and no to_i exists" do
    obj = mock("object")
    obj.should_receive(:to_i).and_return(nil)
    lambda { Integer(obj) }.should raise_error(TypeError)
  end

  it "raises a FloatDomainError when passed NaN" do
    lambda { Integer(0.0/0.0) }.should raise_error(FloatDomainError)
  end

  it "raises a FloatDomainError when passed Infinity" do
    lambda { Integer(1.0/0.0) }.should raise_error(FloatDomainError)
  end
end

describe "Integer() given a String", :shared => true do
  it "raises an ArgumentError if the String is a null byte" do
    lambda { Integer("\0") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if the String starts with a null byte" do
    lambda { Integer("\01") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if the String ends with a null byte" do
    lambda { Integer("1\0") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if the String contains a null byte" do
    lambda { Integer("1\01") }.should raise_error(ArgumentError)
  end

  it "ignores leading whitespace" do
    Integer(" 1").should == 1
    Integer("   1").should == 1
  end

  it "ignores trailing whitespace" do
    Integer("1 ").should == 1
    Integer("1   ").should == 1
  end

  it "raises an ArgumentError if there are leading _s" do
    lambda { Integer("_1") }.should raise_error(ArgumentError)
    lambda { Integer("___1") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if there are trailing _s" do
    lambda { Integer("1_") }.should raise_error(ArgumentError)
    lambda { Integer("1___") }.should raise_error(ArgumentError)
  end

  it "ignores an embedded _" do
    Integer("1_1").should == 11
  end

  it "raises an ArgumentError if there are multiple embedded _s" do
    lambda { Integer("1__1") }.should raise_error(ArgumentError)
    lambda { Integer("1___1") }.should raise_error(ArgumentError)
  end

  it "ignores a single leading +" do
    Integer("+1").should == 1
  end

  it "raises an ArgumentError if there is a space between the + and number" do
    lambda { Integer("+ 1") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if there are multiple leading +s" do
    lambda { Integer("++1") }.should raise_error(ArgumentError)
    lambda { Integer("+++1") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if there are trailing +s" do
    lambda { Integer("1+") }.should raise_error(ArgumentError)
    lambda { Integer("1+++") }.should raise_error(ArgumentError)
  end

  it "makes the number negative if there's a leading -" do
    Integer("-1").should == -1
  end

  it "raises an ArgumentError if there are multiple leading -s" do
    lambda { Integer("--1") }.should raise_error(ArgumentError)
    lambda { Integer("---1") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if there are trailing -s" do
    lambda { Integer("1-") }.should raise_error(ArgumentError)
    lambda { Integer("1---") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError if there is a period" do
    lambda { Integer("0.0") }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError for an empty String" do
    lambda { Integer("") }.should raise_error(ArgumentError)
  end

  %w(x X).each do |x|
    it "parses the value as a hex number if there's a leading 0#{x}" do
      Integer("0#{x}1").should == 0x1
      Integer("0#{x}dd").should == 0xdd
    end

    it "is a positive hex number if there's a leading +0#{x}" do
      Integer("+0#{x}1").should == 0x1
      Integer("+0#{x}dd").should == 0xdd
    end

    it "is a negative hex number if there's a leading -0#{x}" do
      Integer("-0#{x}1").should == -0x1
      Integer("-0#{x}dd").should == -0xdd
    end

    it "raises an ArgumentError if the number cannot be parsed as hex" do
      lambda { Integer("0#{x}g") }.should raise_error(ArgumentError)
    end
  end

  %w(b B).each do |b|
    it "parses the value as a binary number if there's a leading 0#{b}" do
      Integer("0#{b}1").should == 0b1
      Integer("0#{b}10").should == 0b10
    end

    it "is a positive binary number if there's a leading +0#{b}" do
      Integer("+0#{b}1").should == 0b1
      Integer("+0#{b}10").should == 0b10
    end

    it "is a negative binary number if there's a leading -0#{b}" do
      Integer("-0#{b}1").should == -0b1
      Integer("-0#{b}10").should == -0b10
    end

    it "raises an ArgumentError if the number cannot be parsed as binary" do
      lambda { Integer("0#{b}2") }.should raise_error(ArgumentError)
    end
  end

  ["o", "O", ""].each do |o|
    it "parses the value as an octal number if there's a leading 0#{o}" do
      Integer("0#{o}1").should == 0O1
      Integer("0#{o}10").should == 0O10
    end

    it "is a positive octal number if there's a leading +0#{o}" do
      Integer("+0#{o}1").should == 0O1
      Integer("+0#{o}10").should == 0O10
    end

    it "is a negative octal number if there's a leading -0#{o}" do
      Integer("-0#{o}1").should == -0O1
      Integer("-0#{o}10").should == -0O10
    end

    it "raises an ArgumentError if the number cannot be parsed as octal" do
      lambda { Integer("0#{o}9") }.should raise_error(ArgumentError)
    end
  end

  %w(D d).each do |d|
    it "parses the value as a decimal number if there's a leading 0#{d}" do
      Integer("0#{d}1").should == 1
      Integer("0#{d}10").should == 10
    end

    it "is a positive decimal number if there's a leading +0#{d}" do
      Integer("+0#{d}1").should == 1
      Integer("+0#{d}10").should == 10
    end

    it "is a negative decimal number if there's a leading -0#{d}" do
      Integer("-0#{d}1").should == -1
      Integer("-0#{d}10").should == -10
    end

    it "raises an ArgumentError if the number cannot be parsed as decimal" do
      lambda { Integer("0#{d}a") }.should raise_error(ArgumentError)
    end
  end
end

describe "Kernel.Integer" do
  it_behaves_like :kernel_integer, :Integer, Kernel
  it_behaves_like "Integer() given a String", :Integer

  it "is a public method" do
    Kernel.Integer(10).should == 10
  end
end

describe "Kernel#Integer" do
  it_behaves_like :kernel_integer, :Integer, Object.new
  it_behaves_like "Integer() given a String", :Integer

  it "is a private method" do
    Kernel.should have_private_instance_method(:Integer)
  end
end
