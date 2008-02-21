require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML.tagged_classes" do
  it "returns a complete dictionary of taguris paired with classes" do
    YAML.tagged_classes["tag:yaml.org,2002:int"].should == Integer
  end  
end
