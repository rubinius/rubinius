require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Object#class_variable_set" do
  it "is equivalent to self.class.class_variable_set" do
    ObjectSpecs::OIS_CV1.new.class_variable_set(:@@a, 1).should == :cvset 
  end
end
