require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Options#match?" do
  before :each do
    @opt = Rubinius::Options.new
  end

  it "returns the Rubinius::Options::Option instance matching the argument" do
    @opt.on "-a", "--abdc", "desc"
    option = @opt.match? "-a"
    @opt.match?("--abdc").should equal(option)
    option.should be_kind_of(Rubinius::Options::Option)
    option.short.should == "-a"
    option.long.should == "--abdc"
    option.description.should == "desc"
  end
end
