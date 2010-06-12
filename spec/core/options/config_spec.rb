require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#config=" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "sets the config attribute" do
    @opt.config.should be_nil
    @opt.config = :config
    @opt.config.should == :config
  end
end
