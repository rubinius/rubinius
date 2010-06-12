require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options.new" do
  before :each do
    @opt = Rubinius::Options.new("cmd", 20, :config)
  end

  it "sets the banner attribute" do
    @opt.banner.should == "cmd"
  end

  it "sets the config attribute" do
    @opt.config.should == :config
  end

  it "sets the width attribute" do
    @opt.width.should == 20
  end

  it "sets the default width attribute" do
    Rubinius::Options.new.width.should == 30
  end
end
