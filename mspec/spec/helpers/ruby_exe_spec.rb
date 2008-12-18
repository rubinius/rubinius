require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/ruby_exe'
require 'rbconfig'

class RubyExeSpecs
end

describe "#ruby_exe_options" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil

    @ruby_name = Object.const_get :RUBY_NAME
    @ruby_exe_env = ENV['RUBY_EXE']

    @script = RubyExeSpecs.new
  end

  after :all do
    Object.const_set :RUBY_NAME, @ruby_name
    ENV['RUBY_EXE'] = @ruby_exe_env
    $VERBOSE = @verbose
  end

  before :each do
    @script = RubyExeSpecs.new
  end

  it "returns ENV['RUBY_EXE'] when passed :env" do
    ENV['RUBY_EXE'] = "kowabunga"
    @script.ruby_exe_options(:env).should == "kowabunga"
  end

  it "returns 'bin/rbx' when passed :engine and RUBY_NAME is 'rbx'" do
    Object.const_set :RUBY_NAME, 'rbx'
    @script.ruby_exe_options(:engine).should == 'bin/rbx'
  end

  it "returns 'bin/jruby' when passed :engine and RUBY_NAME is 'jruby'" do
    Object.const_set :RUBY_NAME, 'jruby'
    @script.ruby_exe_options(:engine).should == 'bin/jruby'
  end

  it "returns 'ir' when passed :engine and RUBY_NAME is 'ironruby'" do
    Object.const_set :RUBY_NAME, 'ironruby'
    @script.ruby_exe_options(:engine).should == 'ir'
  end

  it "returns RUBY_NAME + $(EXEEXT) when passed :name" do
    bin = RUBY_NAME + (Config::CONFIG['EXEEXT'] || Config::CONFIG['exeext'] || '')
    name = File.join ".", bin
    @script.ruby_exe_options(:name).should == name
  end

  it "returns $(bindir)/$(RUBY_INSTALL_NAME) + $(EXEEXT) when passed :install_name" do
    bin = Config::CONFIG['RUBY_INSTALL_NAME'] + (Config::CONFIG['EXEEXT'] || Config::CONFIG['exeext'] || '')
    name = File.join Config::CONFIG['bindir'], bin
    @script.ruby_exe_options(:install_name).should == name
  end
end

describe "#resolve_ruby_exe" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil

    @name = "ruby_spec_exe"
  end

  before :each do
    @ruby_platform = Object.const_get :RUBY_PLATFORM

    @script = RubyExeSpecs.new
  end

  after :each do
    Object.const_set :RUBY_PLATFORM, @ruby_platform
  end

  after :all do
    $VERBOSE = @verbose
  end

  it "returns the value returned by #ruby_exe_options if it exists and is executable" do
    Object.const_set :RUBY_PLATFORM, "notwindows"
    @script.should_receive(:ruby_exe_options).and_return(@name)
    File.should_receive(:exists?).with(@name).and_return(true)
    File.should_receive(:executable?).with(@name).and_return(true)
    @script.resolve_ruby_exe.should == @name
  end

  it "returns the value returned by #ruby_exe_options if it exists on Windows platforms" do
    Object.const_set :RUBY_PLATFORM, "mswin"
    @script.should_receive(:ruby_exe_options).and_return(@name)
    File.should_receive(:exists?).with(@name).and_return(true)
    File.should_not_receive(:executable?)
    @script.resolve_ruby_exe.should == @name
  end

  it "returns nil if no exe is found" do
    File.should_receive(:exists?).at_least(:once).and_return(false)
    @script.resolve_ruby_exe.should be_nil
  end
end

describe Object, "#ruby_exe" do
  before :all do
    @verbose = $VERBOSE
    $VERBOSE = nil

    @ruby_flags = ENV["RUBY_FLAGS"]
    ENV["RUBY_FLAGS"] = "-w -Q"

    @ruby_exe = Object.const_get :RUBY_EXE
    Object.const_set :RUBY_EXE, 'ruby_spec_exe'

    @file = "some/ruby/file.rb"
    @code = %(some "real" 'ruby' code)

    @script = RubyExeSpecs.new
  end

  after :all do
    Object.const_set :RUBY_EXE, @ruby_exe
    ENV["RUBY_FLAGS"] = @ruby_flags
    $VERBOSE = @verbose
  end

  it "executes the argument if it is a file that exists" do
    File.should_receive(:exists?).with(@file).and_return(true)
    @script.should_receive(:`).with("ruby_spec_exe -w -Q some/ruby/file.rb")
    @script.ruby_exe @file
  end

  it "executes the file with options and arguments" do
    File.should_receive(:exists?).with(@file).and_return(true)
    @script.should_receive(:`).with(
      "ruby_spec_exe -w -Q -w -Cdir some/ruby/file.rb < file.txt")
    @script.ruby_exe @file, :options => "-w -Cdir", :args => "< file.txt"
  end

  it "executes the argument with -e" do
    File.should_receive(:exists?).with(@code).and_return(false)
    @script.should_receive(:`).with(
      %(ruby_spec_exe -w -Q -e "some \\"real\\" 'ruby' code"))
    @script.ruby_exe @code
  end

  it "executes the code with options and arguments" do
    File.should_receive(:exists?).with(@code).and_return(false)
    @script.should_receive(:`).with(
      %(ruby_spec_exe -w -Q -W0 -Cdir -e "some \\"real\\" 'ruby' code" < file.txt))
    @script.ruby_exe @code, :options => "-W0 -Cdir", :args => "< file.txt"
  end

  it "executes with options and arguments but without code or file" do
    @script.should_receive(:`).with("ruby_spec_exe -w -Q -c > file.txt")
    @script.ruby_exe nil, :options => "-c", :args => "> file.txt"
  end
end
