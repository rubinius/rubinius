require File.expand_path('../../../../spec_helper', __FILE__)

describe "Rubinius::Options::Option#description" do
  it "returns the description attribute" do
    option = Rubinius::Options::Option.new(nil, nil, nil, "description", nil)
    option.description.should == "description"
  end
end
