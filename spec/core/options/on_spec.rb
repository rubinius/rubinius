require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#on" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "adds a short option" do
    @opt.should_receive(:add).with("-a", nil, nil, "desc", nil)
    @opt.on("-a", "desc")
  end

  it "adds a short option taking an argument" do
    @opt.should_receive(:add).with("-a", nil, "ARG", "desc", nil)
    @opt.on("-a", "ARG", "desc")
  end

  it "adds a long option" do
    @opt.should_receive(:add).with("-a", nil, nil, "desc", nil)
    @opt.on("-a", "desc")
  end

  it "adds a long option taking an argument" do
    @opt.should_receive(:add).with("-a", nil, nil, "desc", nil)
    @opt.on("-a", "desc")
  end

  it "adds a short and long option" do
    @opt.should_receive(:add).with("-a", nil, nil, "desc", nil)
    @opt.on("-a", "desc")
  end

  it "adds a short and long option taking an argument" do
    @opt.should_receive(:add).with("-a", nil, nil, "desc", nil)
    @opt.on("-a", "desc")
  end

  it "raises Rubinius::Options::OptionError if pass less than 2 arguments" do
    lambda { @opt.on    }.should raise_error(Rubinius::Options::OptionError)
    lambda { @opt.on "" }.should raise_error(Rubinius::Options::OptionError)
  end
end
