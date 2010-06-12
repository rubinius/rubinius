require File.expand_path('../../../spec_helper', __FILE__)
require 'options'

describe "Options#option" do
  before { @o = Options.new }

  it "takes a string with three parts: short and long options and a description" do
    lambda { @o.option '-h --help Help' }.should_not raise_error
  end

  it "does not accept the definition string without leading dashes" do
    lambda { @o.option 'h help Help' }.should raise_error(ArgumentError)
  end

  it "requires that all three parts are present" do
    lambda { @o.option '-h Help'  }.should raise_error(ArgumentError)
    lambda { @o.option '--h Help' }.should raise_error(ArgumentError)
  end

  it "requires that the three parts are in order" do
    lambda { @o.option '--help -h Help' }.should raise_error(ArgumentError)
    lambda { @o.option '-h Help --help' }.should raise_error(ArgumentError)
  end

  it "optionally takes a parameter to designate number of arguments wanted" do
    lambda { @o.option '-f --file File', :one }.should_not raise_error
  end
end
