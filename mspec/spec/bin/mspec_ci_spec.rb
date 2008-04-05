require File.dirname(__FILE__) + '/../../spec_helper'
load File.dirname(__FILE__) + '/../../bin/mspec-ci'

# TODO: specs

describe MSpecCI, "#options" do
  before :each do
    @options = mock("MSpecOptions", :null_object => true)
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecCI.new
  end

  it "adds the config option" do
    @options.should_receive(:add_config)
    @script.options
  end
end
