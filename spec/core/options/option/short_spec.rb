require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinius::Options::Option#short" do
  it "returns the short option attribute" do
    option = Rubinius::Options::Option.new("-a", nil, nil, nil, nil)
    option.short.should == "-a"
  end
end
