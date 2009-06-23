require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Rubinius::Options::Option#long" do
  it "returns the long option attribute" do
    option = Rubinius::Options::Option.new(nil, "--long-option", nil, nil, nil)
    option.long.should == "--long-option"
  end
end
