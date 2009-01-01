require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/match'

describe "Regexp#=~" do
  it_behaves_like(:regexp_match, :=~)
end

describe "Regexp#match" do
  it_behaves_like(:regexp_match, :match)
end

describe "Regexp#~" do
  it "matches against the contents of $_" do
    $_ = "input data"
    (~ /at/).should == 7
  end
end

describe "Regexp#=~ on a successful match" do
  it "returns the index of the first character of the matching region" do
    (/(.)(.)(.)/ =~ "abc").should == 0
  end
end

describe "Regexp#match on a successful match" do
  it "returns a MatchData object" do
    (/(.)(.)(.)/.match "abc").class.should == MatchData
  end
end
