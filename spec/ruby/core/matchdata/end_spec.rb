# -*- encoding: utf-8 -*-

require File.expand_path('../../../spec_helper', __FILE__)

describe "MatchData#end" do
  it "returns the offset of the end of the nth element" do
    match_data = /(.)(.)(\d+)(\d)/.match("THX1138.")
    match_data.end(0).should == 7
    match_data.end(2).should == 3
  end

  it "returns nil when the nth match isn't found" do
    match_data = /something is( not)? (right)/.match("something is right")
    match_data.end(1).should be_nil
  end

  ruby_version_is ""..."1.9" do
    it "returns the offset for multi byte strings" do
      match_data = /(.)(.)(\d+)(\d)/.match("TñX1138.")
      match_data.end(0).should == 8
      match_data.end(2).should == 4
    end

    it "returns the offset for multi byte strings with unicode regexp" do
      match_data = /(.)(.)(\d+)(\d)/u.match("TñX1138.")
      match_data.end(0).should == 8
      match_data.end(2).should == 4
    end
  end

  ruby_version_is "1.9" do
    it "returns the offset for multi byte strings" do
      match_data = /(.)(.)(\d+)(\d)/.match("TñX1138.")
      match_data.end(0).should == 7
      match_data.end(2).should == 3
    end

    it "returns the offset for multi byte strings with unicode regexp" do
      match_data = /(.)(.)(\d+)(\d)/u.match("TñX1138.")
      match_data.end(0).should == 7
      match_data.end(2).should == 3
    end
  end
end
