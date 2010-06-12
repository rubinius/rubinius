require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#banner=" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "sets the banner attribute" do
    @opt.banner.should == ""
    @opt.banner = "banner"
    @opt.banner.should == "banner"
  end
end
