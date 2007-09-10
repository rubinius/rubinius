require File.dirname(__FILE__) + '/../../spec_helper'

describe "Range.last_match" do
  it "returns MatchData instance when not passed arguments" do
    /c(.)t/ =~ 'cat'
    Regexp.last_match.is_a?(MatchData).should == true
  end
  
  it "returns the nth field in this MatchData when passed a Fixnum" do
    /c(.)t/ =~ 'cat'
    Regexp.last_match(1).should == 'a'
  end
end
