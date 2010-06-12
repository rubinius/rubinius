require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#width=" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "sets the width attribute" do
    @opt.width.should == 30
    @opt.width = 20
    @opt.width.should == 20
  end
end
