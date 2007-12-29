require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Object#class_variable_get" do
  it "is equivalent to self.class.class_variable_get" do
    ObjectSpecs::OIS_CV1.new.class_variable_get(:@@a).should == :cvget 
  end
end
