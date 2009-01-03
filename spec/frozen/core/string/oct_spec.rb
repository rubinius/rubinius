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
    "5678".oct.should == 0567

    "0b1010".oct.should == 0b1010
    "0xFF".oct.should == 0xFF
    "0d500".oct.should == 500
  end
  
  ruby_version_is "" ... "1.8.7" do
    it "accepts a sequence of underscores as part of a number" do
      "7__3".oct.should == 073
      "7___3".oct.should == 073
      "7__5".oct.should == 075
    end
  end
  
  ruby_version_is "1.8.7" do
    it "does not accept a sequence of underscores as part of a number" do
      "7__3".oct.should == 07
      "7___3".oct.should == 07
      "7__5".oct.should == 07
    end
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
