require File.dirname(__FILE__) + '/../../spec_helper'
load File.dirname(__FILE__) + '/../../bin/mspec'

# TODO: specs

describe MSpecMain, "#options" do
  before :each do
    @options = mock("MSpecOptions", :null_object => true)
    @options.stub!(:parser).and_return(mock("parser"))
    @options.parser.stub!(:filter!).and_return(["blocked!"])
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
  end

  it "adds the config option" do
    @options.should_receive(:add_config)
    @script.options
  end
end
