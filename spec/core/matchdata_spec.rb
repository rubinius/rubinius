require File.dirname(__FILE__) + '/../spec_helper'

# [], begin, captures, end, inspect, length, offset, post_match, pre_match
# select, size, string, to_a, to_s

describe "MatchData#[]" do
  it "acts as normal array indexing [index]" do
    /(.)(.)(\d+)(\d)/.match("THX1138.")[0].should == 'HX1138'
    /(.)(.)(\d+)(\d)/.match("THX1138.")[1].should == 'H'
    /(.)(.)(\d+)(\d)/.match("THX1138.")[2].should == 'X'
  end

  it "supports accessors [start, length]" do
    /(.)(.)(\d+)(\d)/.match("THX1138.")[1, 2].should == %w|H X|
    /(.)(.)(\d+)(\d)/.match("THX1138.")[-3, 2].should == %w|X 113|
  end

  it "supports ranges [start..end]" do
    /(.)(.)(\d+)(\d)/.match("THX1138.")[1..3].should == %w|H X 113|
  end

  it "provides access to named groups" do
    r = Regexp.new("(?<section>...)(?<code>\d+)")
    m = r.match("THX1138")
    m[:section] == "THX"
    m[:code] == "1138"
  end
end

describe "MatchData#begin" do
  it "returns the offset of the start of the nth element" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(0).should == 1
    /(.)(.)(\d+)(\d)/.match("THX1138.").begin(2).should == 2
  end

end

describe "MatchData#captures" do
  it "returns an array of the match captures" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").captures.should == ["H","X","113","8"]
  end

end

describe "MatchData#end" do
  it "returns the offset of the end of the nth element" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").end(0).should == 7
    /(.)(.)(\d+)(\d)/.match("THX1138.").end(2).should == 3 
  end

end

matchdata_length = shared "MatchData#length | MatchData#size" do |cmd|
  describe "MatchData##{cmd}" do
    it "length should return the number of elements in the match array" do
      /(.)(.)(\d+)(\d)/.match("THX1138.").send(cmd).should == 5
    end
  end
end

describe "MatchData#length" do
  it_behaves_like(matchdata_length, :length)
end

describe "MatchData#size" do
  it_behaves_like(matchdata_length, :size)
end

describe "MatchData#to_s" do
  it "returns the entire matched string" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").to_s.should == "HX1138"
  end
end

describe "MatchData#offset" do
  it "returns a two element array with the begin and end of the nth match" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").offset(0).should == [1, 7]
    /(.)(.)(\d+)(\d)/.match("THX1138.").offset(4).should == [6, 7]
  end

end

describe "MatchData#post_match" do
  it "returns the string after the match equiv. special var $'" do
    /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").post_match.should == ': The Movie' 
    $'.should == ': The Movie'
  end

end

describe "MatchData#pre_match" do
  it "returns the string before the match, equiv. special var $`" do
    /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").pre_match.should == 'T'
    $`.should == 'T'
  end

end

describe "MatchData#values_at" do
  it "returns an array of the matching value" do
    /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").values_at(0, 2, -2).should == ["HX1138", "X", "113"]
  end

end

describe "MatchData#select" do
  it "yields the contents of the match array to a block" do
     /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").select { |x| x }.should == ["HX1138", "H", "X", "113", "8"]
  end

end

describe "MatchData#string" do
  it "returns a frozen copy of the match string" do
    str = /(.)(.)(\d+)(\d)/.match("THX1138.").string
    str.should == "THX1138."
    str.frozen?.should == true
  end

end

describe "MatchData#to_a" do
  it "returns an array of matches" do
    /(.)(.)(\d+)(\d)/.match("THX1138.").to_a.should == ["HX1138", "H", "X", "113", "8"]
  end  
end
