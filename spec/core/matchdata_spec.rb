require File.dirname(__FILE__) + '/../spec_helper'

# [], begin, captures, end, inspect, length, offset, post_match, pre_match
# select, size, string, to_a, to_s

context "MatchData instance method" do
  specify "[] should act as normal array indexing [index]" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138.")[0],
        /(.)(.)(\d+)(\d)/.match("THX1138.")[1],
        /(.)(.)(\d+)(\d)/.match("THX1138.")[2]
      ]
    end.should == ["HX1138","H","X"]
  end

  specify "[] should support accessors [start, length]" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138.")[1, 2],
        /(.)(.)(\d+)(\d)/.match("THX1138.")[-3, 2]
      ]
    end.should == [["H","X"],["X","113"]]
  end

  specify "[] should support ranges [start..end]" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.")[1..3]
    end.should == ["H", "X", "113"]
  end

  specify "begin(index) should return the offset of the start of the nth element" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138.").begin(0),
        /(.)(.)(\d+)(\d)/.match("THX1138.").begin(2)
      ]
    end.should == [1,2]
  end

  specify "captures should return and array of the match captures" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").captures
    end.should == ["H","X","113","8"]
  end

  specify "end(index) should return the offset of the end of the nth element" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138.").end(0),
        /(.)(.)(\d+)(\d)/.match("THX1138.").end(2) 
      ]
    end.should == [7,3]
  end

  specify "length should return the number of elements in the match array" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").length
    end.should == 5
  end

  specify "size should return the number of elements in the match array" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").size
    end.should == 5
  end

  specify "to_s should return the entire matched string" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").to_s
    end.should == "HX1138"
  end

  specify "offset(index) should return a two element array with the begin and end of the nth match" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138.").offset(0),
        /(.)(.)(\d+)(\d)/.match("THX1138.").offset(4)
      ]
    end.should == [[1,7],[6,7]]
  end

  specify "post_match should return the string after the match equiv. special var $'" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").post_match,
        $'
      ]
    end.should == [": The Movie",": The Movie"]
  end

  specify "pre_match should return the string before the match, equiv. special var $`" do
    example do
      [
        /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").pre_match,
        $`
      ]
    end.should == ["T","T"]
  end

  specify "values_at([index]*) should return an array of the matching value" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").values_at(0, 2, -2)
    end.should == ["HX1138", "X", "113"]
  end

  specify "select (depreciated) should yield the contents of the match array to a block" do
    example do
       /(.)(.)(\d+)(\d)/.match("THX1138: The Movie").select { |x| x }
    end.should == ["HX1138", "H", "X", "113", "8"]
  end

  specify "string should return a frozen copy of the match string" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").string
    end.should == "THX1138."
  end

  specify "to_a returns an array of matches" do
    example do
      /(.)(.)(\d+)(\d)/.match("THX1138.").to_a
    end.should == ["HX1138", "H", "X", "113", "8"]
  end
end

