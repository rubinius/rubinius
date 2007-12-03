require File.dirname(__FILE__) + '/../../spec_helper'

describe "SystemCallError.new" do  
  it "requires at least one argumentt" do
    lambda { SystemCallError.new }.should raise_error(ArgumentError)
  end

  it "takes an optional errno argument" do
    SystemCallError.should_be_ancestor_of(SystemCallError.new("message",1).class)
  end
end

describe "SystemCallError#errno" do
  it "returns nil when no errno given" do
    SystemCallError.new("message").errno.should == nil
  end  
  
  it "returns the errno given as optional argument to new" do
    SystemCallError.new("message", 42).errno.should == 42
  end
end
