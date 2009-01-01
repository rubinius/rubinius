require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Struct#length" do
  it "returns the number of attributes" do
    Struct::Car.new('Cadillac', 'DeVille').length.should == 3
    Struct::Car.new.length.should == 3
  end
end
