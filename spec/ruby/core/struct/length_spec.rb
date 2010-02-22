require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Struct#length" do
  it "returns the number of attributes" do
    Struct::Car.new('Cadillac', 'DeVille').length.should == 3
    Struct::Car.new.length.should == 3
  end
end
