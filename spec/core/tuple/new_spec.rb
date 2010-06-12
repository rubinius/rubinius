require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Tuple.new" do
  it "creates a Tuple of specified size" do
    Rubinius::Tuple.new(2).fields.should == 2
    Rubinius::Tuple.new(2).size.should == 2
    Rubinius::Tuple.new(2).length.should == 2
  end

  it "creates a Tuple of zero size" do
    Rubinius::Tuple.new(0).fields.should == 0
    Rubinius::Tuple.new(0).size.should == 0
    Rubinius::Tuple.new(0).length.should == 0
  end
end
