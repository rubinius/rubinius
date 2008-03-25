require File.dirname(__FILE__) + '/../../spec_helper'

describe "Tuple.template" do
  it "returns a Tuple of specified length initialized to object" do
    t = Tuple.template(5, -1)
    t.size.should == 5
    t.each { |e| e.should == -1 }
  end
end
