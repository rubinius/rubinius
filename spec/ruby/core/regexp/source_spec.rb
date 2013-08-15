require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  require File.expand_path("../versions/source_1.9", __FILE__)
end

describe "Regexp#source" do
  it "returns the original string of the pattern" do
    /ab+c/ix.source.should == "ab+c"
    /x(.)xz/.source.should == "x(.)xz"
  end
end
