require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#to_i(base=10)" do
  it "should treat leading characters of self as a integer with base base (2, 8, 10, 16)" do
    "12345".to_i.should == 12345
    "99 red balloons".to_i.should == 99
    "0a".to_i.should == 0
    "0a".to_i(16).should == 10
    "1100101".to_i(2).should == 101
    "1100101".to_i(8).should == 294977
    "1100101".to_i(10).should == 1100101
    "1100101".to_i(16).should == 17826049

    " 12345".to_i.should == 12345
    "0b1100101".to_i(0).should == 101
    "0B1100101".to_i(0).should == 101
    "0o1100101".to_i(0).should == 294977
    "0O1100101".to_i(0).should == 294977
    "0x1100101".to_i(0).should == 17826049
    "01100101".to_i(0).should == 294977
    "1100101".to_i(0).should == 1100101
    "1100101".to_i(2).should == 101
    "0b1100101".to_i(2).should == 101
    "0B1000101".to_i(2).should == 69
    "0b".to_i(2).should == 0
    "1".to_i(2).should == 1
    "0o1110101".to_i(8).should == 299073
    "0O1101101".to_i(8).should == 295489
    "0o".to_i(8).should == 0
    "0".to_i(8).should == 0
    "1".to_i(8).should == 1
    "0x1100101".to_i(16).should == 17826049
    "0x1100101".to_i(16).should == 17826049
    "0x".to_i(16).should == 0
    "1".to_i(16).should == 1
  end

  it "should take an optional sign" do
    "-45.67 degrees".to_i.should == -45
    "+45.67 degrees".to_i.should == 45
  end
  
  it "should return 0 if the conversion fails" do
    "hello".to_i.should == 0
    "".to_i.should == 0
    "".to_i(0).should == 0
    "".to_i(10).should == 0
  end
  
  it "should raise an ArgumentError if base is smaller than 0" do
    should_raise(ArgumentError) { "101".to_i(-5) }
  end
end