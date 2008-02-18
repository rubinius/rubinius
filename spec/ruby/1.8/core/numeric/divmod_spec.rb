require File.dirname(__FILE__) + '/../../spec_helper'

# Returns an array containing the quotient and modulus obtained
# by dividing num by aNumeric. If q, r = x.divmod(y), then
#    q = floor(float(x)/float(y))
#    x = q*y + r

describe "Numeric#divmod" do
  it "returns self divmod other" do
    (3**33).divmod(100).should == [55590605665555, 23]
  end

  it "returns an Integer [quotient, modulus] when operating with Integers" do
    13.divmod(4).should == [3,1]
    4.divmod(13).should == [0,4]
  end

  it "returns [quotent, modulus] where modulus may be Float or Integer when operating with Integers and Floats" do
    13.divmod(4.0).should == [3,1]
    4.divmod(13.0).should == [0,4]
    a, b = 13.divmod(4.6)
    a.should == 2
    b.should be_close(3.8, TOLERANCE)
  end

  it "returns a Float [quotient, modulus] when operating with Floats" do
    13.0.divmod(4.0).should == [3.0,1.0]
    4.0.divmod(13).should == [0.0,4.0]
    y = 1 / 12.0
    0.58.divmod(y).first.should == 6
    0.67.divmod(y).first.should == 8
  end
  
  ruby_bug do
    # MRI 1.8.6 bug Numeric#divmod bug
    # http://rubyforge.org/tracker/?func=detail&atid=1698&aid=14540&group_id=426
    it "returns a Float [quotient, modulus] when operating with Floats" do
      y = 1 / 12.0
      0.59.divmod(y).first.should == 7
      0.63.divmod(y).first.should == 7
      0.66.divmod(y).first.should == 7
    end
  end

  it "raises an ArgumentError when not passed one argument" do
    lambda { 13.divmod       }.should raise_error(ArgumentError)
    lambda { 13.divmod(1, 2) }.should raise_error(ArgumentError)
  end

  it "raises a ZeroDivisionError when passed 0" do
    lambda { 13.divmod(0)      }.should raise_error(ZeroDivisionError)
  end

  it "raises a TypeError when not passed a Numeric type" do
    lambda { 13.divmod(nil)    }.should raise_error(TypeError)
    lambda { 13.divmod('test') }.should raise_error(TypeError)
    lambda { 13.divmod(true)   }.should raise_error(TypeError)
  end
end
