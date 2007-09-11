require File.dirname(__FILE__) + '/../../spec_helper'

describe "Exception#inspect" do
  it "returns '#<Exception: Exception>' when no message given" do
    Exception.new.inspect.should == "#<Exception: Exception>"
  end
  
  it "includes message when given" do
    [Exception.new("foobar").inspect].should == ["#<Exception: foobar>"]
  end
end
