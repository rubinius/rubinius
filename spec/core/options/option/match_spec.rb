require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinius::Options::Option#match?" do
  before :each do
    @opt = Rubinius::Options::Option.new("-a", "--bdc", "ARG", "desc", :block)
  end

  it "returns true if the argument matches the short option" do
    @opt.match?("-a").should be_true
  end

  it "returns true if the argument matches the long option" do
    @opt.match?("--bdc").should be_true
  end

  it "returns false if the argument matches neither the short nor long option" do
    @opt.match?("-b").should be_false
    @opt.match?("-abdc").should be_false
  end
end
