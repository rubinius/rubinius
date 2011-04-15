require File.expand_path('../../../spec_helper', __FILE__)

describe "Regexp#===" do
  it "is true if there is a match" do
    (/abc/ === "aabcc").should == true
  end

  it "is false if there is no match" do
    (/abc/ === "xyz").should == false
  end
end
