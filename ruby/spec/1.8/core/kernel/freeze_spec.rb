require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

compliant_on :ruby, :jruby do
  describe "Kernel#freeze" do
    it "prevents self from being further modified" do
      o = mock('o')
      o.frozen?.should == false
      o.freeze
      o.frozen?.should == true
    end
  
    it "has no effect on immediate values" do
      a = nil
      b = true
      c = false
      d = 1
      a.freeze
      b.freeze
      c.freeze
      d.freeze
      a.frozen?.should == false
      b.frozen?.should == false
      c.frozen?.should == false
      d.frozen?.should == false
    end
  end
end
