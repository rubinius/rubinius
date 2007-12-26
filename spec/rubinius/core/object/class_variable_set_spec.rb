require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

extension :rubinius do
  describe "Object#class_variable_set" do
    it "is equivalent to self.class.class_variable_set" do
      ObjectSpecs::OIS_CV1.new.class_variable_set(:@@a, 1).should == :cvset 
    end
  end
end
