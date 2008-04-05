require File.dirname(__FILE__) + '/../../spec_helper'
load File.dirname(__FILE__) + '/../../bin/mspec-run'

# TODO: specs

describe MSpecRun, "#options" do
  before :each do
    @stdout, $stdout = $stdout, CaptureOutput.new

    @options = mock("MSpecOptions", :null_object => true)
    @options.stub!(:parse).and_return(["blocked!"])
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecRun.new
  end

  after :each do
    $stdout = @stdout
  end

  it "adds the config option" do
    @options.should_receive(:add_config)
    @script.options
  end

  it "exits if there are no files to process" do
    @options.should_receive(:parse).and_return([])
    @script.should_receive(:exit)
    @script.options
    $stdout.should =~ /No files specified/
  end
end
