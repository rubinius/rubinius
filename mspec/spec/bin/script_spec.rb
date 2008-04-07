require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../bin/script'

describe MSpecScript, ".config" do
  it "returns a Hash" do
    MSpecScript.config.should be_kind_of(Hash)
  end
end

describe MSpecScript, ".set" do
  it "sets the config hash key, value" do
    MSpecScript.set :a, 10
    MSpecScript.config[:a].should == 10
  end
end

describe MSpecScript, "#config" do
  it "returns the MSpecScript config hash" do
    MSpecScript.set :b, 5
    MSpecScript.new.config[:b].should == 5
  end

  it "returns the MSpecScript config hash from subclasses" do
    class MSSClass < MSpecScript; end
    MSpecScript.set :b, 5
    MSSClass.new.config[:b].should == 5
  end
end

describe MSpecScript, ".main" do
  before :each do
    @script = mock("MSpecScript", :null_object => true)
    MSpecScript.stub!(:new).and_return(@script)
  end

  it "creates an instance of MSpecScript" do
    MSpecScript.should_receive(:new).and_return(@script)
    MSpecScript.main
  end

  it "attempts to load the 'default.mspec' script" do
    @script.should_receive(:load).with('default.mspec')
    MSpecScript.main
  end

  it "attempts to load the '~/.mspecrc' script" do
    @script.should_receive(:load).with('~/.mspecrc')
    MSpecScript.main
  end

  it "calls the #options method on the script" do
    @script.should_receive(:options)
    MSpecScript.main
  end

  it "calls the #signals method on the script" do
    @script.should_receive(:signals)
    MSpecScript.main
  end

  it "calls the #register method on the script" do
    @script.should_receive(:register)
    MSpecScript.main
  end

  it "calls the #run method on the script" do
    @script.should_receive(:run)
    MSpecScript.main
  end
end

describe MSpecScript, "#initialize" do
  before :each do
    @config = MSpecScript.new.config
  end

  it "sets the default config values" do
    @config[:tags_dir].should  == 'spec/tags'
    @config[:formatter].should == DottedFormatter
    @config[:includes].should  == []
    @config[:excludes].should  == []
    @config[:patterns].should  == []
    @config[:xpatterns].should == []
    @config[:tags].should      == []
    @config[:xtags].should     == []
    @config[:atags].should     == []
    @config[:astrings].should  == []
    @config[:abort].should     == true
  end
end

describe MSpecScript, "#load" do
  # TODO: specs
end

describe MSpecScript, "#register" do
  # TODO: specs
end

describe MSpecScript, "#signals" do
  before :each do
    @script = MSpecScript.new
    @abort = @script.config[:abort]
  end

  after :each do
    @script.config[:abort] = @abort
  end

  it "traps the INT signal if config[:abort] is true" do
    Signal.should_receive(:trap).with("INT")
    @script.config[:abort] = true
    @script.signals
  end

  it "does not trap the INT signal if config[:abort] is not true" do
    Signal.should_not_receive(:trap).with("INT")
    @script.config[:abort] = false
    @script.signals
  end
end
