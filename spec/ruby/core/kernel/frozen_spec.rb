require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#frozen?" do
  it "returns true if self is frozen" do
    o = mock('o')
    p = mock('p')
    p.freeze
    o.frozen?.should == false
    p.frozen?.should == true
  end
end
