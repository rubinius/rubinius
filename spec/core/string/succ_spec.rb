require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#succ" do
  it "returns an empty string for empty strings" do
    "".succ.should == ""
  end
  
  it "returns the successor by increasing the rightmost alphanumeric (digit => digit, letter => letter with same case)" do
    "abcd".succ.should == "abce"
    "THX1138".succ.should == "THX1139"
    
    "<<koala>>".succ.should == "<<koalb>>"
    "==A??".succ.should == "==B??"
  end
  
  it "increases non-alphanumerics (via ascii rules) if there are no alphanumerics" do
    "***".succ.should == "**+"
    "**`".succ.should == "**a"
  end
  
  it "increases the next best alphanumeric (jumping over non-alphanumerics) if there is a carry" do
    "dz".succ.should == "ea"
    "HZ".succ.should == "IA"
    "49".succ.should == "50"
    
    "izz".succ.should == "jaa"
    "IZZ".succ.should == "JAA"
    "699".succ.should == "700"
    
    "6Z99z99Z".succ.should == "7A00a00A"
    
    "1999zzz".succ.should == "2000aaa"
    "NZ/[]ZZZ9999".succ.should == "OA/[]AAA0000"
  end

  it "increases the next best character if there is a carry for non-alphanumerics" do
    "(\xFF".succ.should == ")\x00"
    "`\xFF".succ.should == "a\x00"
    "<\xFF\xFF".succ.should == "=\x00\x00"
  end

  it "adds an additional character (just left to the last increased one) if there is a carry and no character left to increase" do
    "z".succ.should == "aa"
    "Z".succ.should == "AA"
    "9".succ.should == "10"
    
    "zz".succ.should == "aaa"
    "ZZ".succ.should == "AAA"
    "99".succ.should == "100"

    "9Z99z99Z".succ.should == "10A00a00A"

    "ZZZ9999".succ.should == "AAAA0000"
    "/[]ZZZ9999".succ.should == "/[]AAAA0000"
    "Z/[]ZZZ9999".succ.should == "AA/[]AAA0000"
    
    # non-alphanumeric cases
    "\xFF".succ.should == "\x01\x00"
    "\xFF\xFF".succ.should == "\x01\x00\x00"
  end

  it "returns subclass instances when called on a subclass" do
    MyString.new("").succ.class.should == MyString
    MyString.new("a").succ.class.should == MyString
    MyString.new("z").succ.class.should == MyString
  end
  
  it "taints the result if self is tainted" do
    ["", "a", "z", "Z", "9", "\xFF", "\xFF\xFF"].each do |s|
      s.taint.succ.tainted?.should == true
    end
  end
end

describe "String#succ!" do
  it "is equivalent to succ, but modifies self in place (still returns self)" do
    ["", "abcd", "THX1138"].each do |s|
      r = s.succ
      s.succ!.equal?(s).should == true
      s.should == r
    end
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "".freeze.succ! }
    should_raise(TypeError) { "abcd".freeze.succ! }
  end
end
