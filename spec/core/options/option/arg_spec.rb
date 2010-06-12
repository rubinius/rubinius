require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinius::Options::Option#arg?" do
  it "returns true if arg attribute is not nil" do
    Rubinius::Options::Option.new(nil, nil, "ARG", nil, nil).arg?.should be_true
  end

  it "returns false if arg attribute is nil" do
    Rubinius::Options::Option.new(nil, nil, nil, nil, nil).arg?.should be_false
  end
end

describe "Rubinius::Options::Option#arg" do
  it "returns the argument string" do
    option = Rubinius::Options::Option.new(nil, nil, "ARG", nil, nil)
    option.arg.should == "ARG"
  end
end
