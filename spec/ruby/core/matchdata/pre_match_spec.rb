require File.expand_path('../../../spec_helper', __FILE__)

describe "MatchData#pre_match" do
  it "returns the string before the match, equiv. special var $`" do
    /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").pre_match.should == 'T'
    $`.should == 'T'
  end

  it "keeps taint status from the source string" do
    str = "THX1138: The Movie"
    str.taint
    res = /(.)(.)(\d+)(\d)/.match(str).pre_match
    res.tainted?.should be_true
    $`.tainted?.should be_true
  end

  it "keeps untrusted status from the source string" do
    str = "THX1138: The Movie"
    str.untrust
    res = /(.)(.)(\d+)(\d)/.match(str).pre_match
    res.untrusted?.should be_true
    $`.untrusted?.should be_true
  end
end
