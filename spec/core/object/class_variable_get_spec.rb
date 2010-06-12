require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Object#class_variable_get" do
  it "is equivalent to self.class.class_variable_get" do
    ObjectSpecs::OIS_CV1.new.class_variable_get(:@@a).should == :cvget 
  end
end
