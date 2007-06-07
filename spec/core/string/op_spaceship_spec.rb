require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#<=>(other_string)" do
  specify "should return -1 when self is less than other" do
    ("this" <=> "those").should == -1
  end

  specify "should return 0 when self is equal to other" do
    ("yep" <=> "yep").should == 0
  end

  specify "should return 1 when self is greater than other" do
    ("yoddle" <=> "griddle").should == 1
  end
  
  specify "should consider string that comes lexicographically first to be less if strings have same size" do
    ("aba" <=> "abc").should == -1
    ("abc" <=> "aba").should == 1
  end

  specify "should consider shorter string to be less if longer string starts with shorter one" do
    ("abc" <=> "abcd").should == -1
    ("abcd" <=> "abc").should == 1
  end

  specify "should compare shorter string with corresponding number of first chars of longer string" do
    ("abx" <=> "abcd").should == 1
    ("abcd" <=> "abx").should == -1
  end
end

describe "String#<=>(obj)" do
  specify "should return nil if obj does not respond to to_str" do
    ("abc" <=> 1).should == nil
    ("abc" <=> :abc).should == nil
    ("abc" <=> Object.new).should == nil
  end
  
  specify "should return nil if obj does not respond to <=>" do
    module StringSpec
      class TestObject
        def to_str
          ""
        end
      end
    end
    
    ("abc" <=> StringSpec::TestObject.new).should == nil
  end
  
  specify "should compare the obj and self by calling <=> on obj and turning the result around" do
    module StringSpec
      class TestObject3
        def to_str
          ""
        end
        
        def <=>(arg)
          1 # This is here only for the sake of testing!
        end
      end
    end
    
    ("abc" <=> StringSpec::TestObject3.new).should == -1
    ("xyz" <=> StringSpec::TestObject3.new).should == -1
  end
end