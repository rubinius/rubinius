require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

compliant_on :ruby, :jruby, :ir do
  describe "Array#frozen?" do
    it "returns true if array is frozen" do
      a = [1, 2, 3]
      a.frozen?.should == false
      a.freeze
      a.frozen?.should == true
    end
  end
end
