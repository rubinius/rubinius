require File.dirname(__FILE__) + '/../../spec_helper'

@regexp_match = shared "Regexp#=~" do |cmd|
  describe "Regexp##{cmd}" do
    it "returns the first position of the match" do
      (/(.)(.)(.)/ =~ "abc").should == 0
      $~.begin(0).should == 0
    end

    it "returns nil if there is no match" do
      /xyz/ =~ "abxyc"
      $~.should == nil
    end
  end
end

describe "Regexp#=~" do
  it_behaves_like(@regexp_match, :=~)
end

describe "Regexp#match" do
  it_behaves_like(@regexp_match, :match)
end

describe "Regexp#~" do
  it "matchs against the contents of $_" do
    $_ = "input data"
    (~ /at/).should == 7
  end
end
