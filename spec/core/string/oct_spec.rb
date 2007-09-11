require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

# Note: We can't completely spec this in terms of to_int() because hex()
# allows the base to be changed by a base specifier in the string.
# See http://groups.google.com/group/ruby-core-google/browse_frm/thread/b53e9c2003425703
describe "String#oct" do
  it "treats leading characters of self as a string of oct digits" do
    "0".oct.should == 0
    "77".oct.should == 077
    "78".oct.should == 7
    "077".oct.should == 077
    "0o".oct.should == 0

    "755_333".oct.should == 0755_333
    "7__3".oct.should == 073
    "5678".oct.should == 0567

    "0b1010".oct.should == 0b1010
    "0xFF".oct.should == 0xFF
    "0d500".oct.should == 500
  end
  
  it "takes an optional sign" do
    "-1234".oct.should == -01234
    "+1234".oct.should == 01234
  end
  
  it "returns 0 on error" do
    "".oct.should == 0
    "+-5".oct.should == 0
    "wombat".oct.should == 0
  end
end
