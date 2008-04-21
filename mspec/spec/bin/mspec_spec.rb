require File.dirname(__FILE__) + '/../../spec_helper'
load File.dirname(__FILE__) + '/../../bin/mspec'

class MMConfig < Hash
  def initialize
    self[:includes] = []
    self[:requires] = []
    self[:target]   = 'ruby'
    self[:flags]    = []
    self[:command]  = nil
    self[:options]  = []
  end
end

def new_option
  config = MMConfig.new
  return MSpecOptions.new(config, "spec"), config
end

describe MSpecMain, "#options" do
  before :each do
    @options = mock("MSpecOptions", :null_object => true)
    @options.stub!(:parser).and_return(mock("parser"))
    @options.parser.stub!(:filter!).and_return(["blocked!"])
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
  end

  it "enables the config option" do
    @options.should_receive(:add_config)
    @script.options
  end

  it "enables the target options" do
    @options.should_receive(:add_targets)
    @script.options
  end

  it "enables the version option" do
    @options.should_receive(:add_version)
    @script.options
  end
end

describe MSpecMain, "#run" do
  before :each do
    @script = MSpecMain.new
  end

  it "sets MSPEC_RUNNER = '1' in the environment" do
    @script.stub!(:exec)
    ENV["MSPEC_RUNNER"] = "0"
    @script.run
    ENV["MSPEC_RUNNER"].should == "1"
  end

  it "uses exec to invoke the runner script" do
    @script.should_receive(:exec).with("ruby", "mspec/bin/mspec-run")
    @script.run
  end
end

describe "The -D, --gdb option" do
  before :each do
    @options, @config = new_option
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
    @script.stub!(:config).and_return(@config)
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-D", "--gdb", an_instance_of(String))
    @script.options
  end

  it "sets flags to --gdb" do
    ["-D", "--gdb"].each do |opt|
      @config[:flags] = []
      @script.options [opt]
      @config[:flags].should include("--gdb")
    end
  end
end

describe "The -A, --valgrind option" do
  before :each do
    @options, @config = new_option
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
    @script.stub!(:config).and_return(@config)
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-A", "--valgrind", an_instance_of(String))
    @script.options
  end

  it "sets flags to --valgrind" do
    ["-A", "--valgrind"].each do |opt|
      @config[:flags] = []
      @script.options [opt]
      @config[:flags].should include("--valgrind")
    end
  end
end

describe "The --warnings option" do
  before :each do
    @options, @config = new_option
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
    @script.stub!(:config).and_return(@config)
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("--warnings", an_instance_of(String))
    @script.options
  end

  it "sets flags to -w" do
    @config[:flags] = []
    @script.options ["--warnings"]
    @config[:flags].should include("-w")
  end

  it "set OUTPUT_WARNINGS = '1' in the environment" do
    ENV['OUTPUT_WARNINGS'] = '0'
    @script.options ["--warnings"]
    ENV['OUTPUT_WARNINGS'].should == '1'
  end
end

describe "The -h, --help option" do
  before :each do
    @options, @config = new_option
    MSpecOptions.stub!(:new).and_return(@options)
    @script = MSpecMain.new
    @script.stub!(:config).and_return(@config)
  end

  it "is enabled by #options" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-h", "--help", an_instance_of(String))
    @script.options
  end

  it "prints help and exits" do
    @script.should_receive(:puts).twice
    @script.should_receive(:exit).twice
    ["-h", "--help"].each do |opt|
      @script.options [opt]
    end
  end
end
