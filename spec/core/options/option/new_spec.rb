require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinis::Options::Option.new" do
  before :each do
    @opt = Rubinius::Options::Option.new("-a", "--bdc", "ARG", "desc", :block)
  end

  it "sets the short attribute" do
    @opt.short.should == "-a"
  end

  it "sets the long attribute" do
    @opt.long.should == "--bdc"
  end

  it "sets the arg attribute" do
    @opt.arg.should == "ARG"
  end

  it "sets the description attribute" do
    @opt.description.should == "desc"
  end

  it "sets the block attribute" do
    @opt.block.should == :block
  end
end
