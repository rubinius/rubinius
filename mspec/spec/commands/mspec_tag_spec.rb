require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/runner/mspec'
require 'mspec/commands/mspec-tag'

describe MSpecTag, "#options" do
  before :each do
    @stdout, $stdout = $stdout, IOStub.new

    @argv = ["a", "b"]
    @options, @config = new_option
    MSpecOptions.stub!(:new).and_return(@options)

    @script = MSpecTag.new
    @script.stub!(:config).and_return(@config)
  end

  after :each do
    $stdout = @stdout
  end

  it "enables the filter options" do
    @options.should_receive(:add_filters)
    @script.options @argv
  end

  it "enables the config option" do
    @options.should_receive(:add_config)
    @script.options @argv
  end

  it "provides a custom action (block) to the config option" do
    @script.should_receive(:load).with("cfg.mspec")
    @script.options ["-B", "cfg.mspec", "a"]
  end

  it "enables the name option" do
    @options.should_receive(:add_name)
    @script.options @argv
  end

  it "enables the dry run option" do
    @options.should_receive(:add_pretend)
    @script.options @argv
  end

  it "enables the interrupt single specs option" do
    @options.should_receive(:add_interrupt)
    @script.options @argv
  end

  it "enables the formatter options" do
    @options.should_receive(:add_formatters)
    @script.options @argv
  end

  it "enables the verbose option" do
    @options.should_receive(:add_verbose)
    @script.options @argv
  end

  it "enables the tagging options" do
    @options.should_receive(:add_tagging)
    @script.options @argv
  end

  it "enables the version option" do
    @options.should_receive(:add_version)
    @script.options @argv
  end

  it "enables the help option" do
    @options.should_receive(:add_help)
    @script.options @argv
  end

  it "exits if there are no files to process" do
    @options.should_receive(:parse).and_return([])
    @script.should_receive(:exit)
    @script.options
    $stdout.should =~ /No files specified/
  end
end

describe MSpecTag, "#run" do
  before :each do
    MSpec.stub!(:process)

    stat = mock("stat")
    stat.stub!(:file?).and_return(true)
    stat.stub!(:directory?).and_return(false)
    File.stub!(:expand_path)
    File.stub!(:stat).and_return(stat)

    options = mock("MSpecOptions", :null_object => true)
    options.stub!(:parse).and_return(["one", "two"])
    MSpecOptions.stub!(:new).and_return(options)

    @config = { }
    @script = MSpecTag.new
    @script.stub!(:exit)
    @script.stub!(:config).and_return(@config)
    @script.options
  end

  it "registers the tags patterns" do
    @config[:tags_patterns] = [/spec/, "tags"]
    MSpec.should_receive(:register_tags_patterns).with([/spec/, "tags"])
    @script.run
  end

  it "registers the files to process" do
    MSpec.should_receive(:register_files).with(["one", "two"])
    @script.run
  end

  it "processes the files" do
    MSpec.should_receive(:process)
    @script.run
  end

  it "exits with the exit code registered with MSpec" do
    MSpec.stub!(:exit_code).and_return(7)
    @script.should_receive(:exit).with(7)
    @script.run
  end
end
