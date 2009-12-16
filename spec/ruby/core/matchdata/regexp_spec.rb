require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.8" do
  describe "MatchData#regexp" do
    it "returns a Regexp object" do
      m = 'haystack'.match(/hay/)
      m.regexp.should be_an_instance_of(Regexp)
    end

    it "returns the pattern used in the match" do
      m = 'haystack'.match(/hay/)
      m.regexp.should == /hay/
    end
  end
end
