require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#chop" do
  it "should return a new string with the last char removed" do
    "string\n\r".chop.should == "string\n"
    "string\n".chop.should == "string"
    "string".chop.should == "strin"
  end
  
  it "should remove the last two chars it self ends with \\r\\n" do
    "string\r\n".chop.should == "string"
  end
  
  it "should return an empty string if self is empty" do
    "".chop.should == ""
  end
end

describe "String#chop!" do
  it "should modify self in place" do
    a = "string\n\r"
    a.chop!.should == "string\n"
    a.should == "string\n"
    
    b = "string\r\n"
    b.chop!.should == "string"
    b.should == "string"
  end
  
  it "should return nil if no modifications were made" do
    a = ""
    a.chop!.should == nil
    a.should == ""
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "string\n\r"
      a.freeze
      a.chop!
    end
  end
end