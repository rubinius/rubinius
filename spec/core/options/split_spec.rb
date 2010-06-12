require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#split" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "breaks a string at the nth character" do
    opt, arg, rest = @opt.split "-bdc", 2
    opt.should == "-b"
    arg.should == "dc"
    rest.should == "dc"
  end

  it "returns nil for arg if there are no characters left" do
    opt, arg, rest = @opt.split "-b", 2
    opt.should == "-b"
    arg.should == nil
    rest.should == ""
  end
end
