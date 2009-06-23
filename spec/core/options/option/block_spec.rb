require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Rubinius::Options::Option#block" do
  it "returns the block attribute" do
    option = Rubinius::Options::Option.new(nil, nil, nil, nil, :block)
    option.block.should == :block
  end
end
