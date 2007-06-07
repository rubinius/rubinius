require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#==(other_string)" do
  it "should return true if self <=> string returns 0" do
    ('hello' == 'hello').should == true
  end
  
  it "should return false if self <=> string does not return 0" do
    ("more" == "MORE").should == false
    ("less" == "greater").should == false
  end
end

describe "String#==(obj)" do
  it "should return false if obj does not respond to to_str" do
    ('hello' == 5).should == false
    ('hello' == :hello).should == false
    ('hello' == Object.new).should == false
  end
  
  it "should return obj == self if obj responds to to_str" do
    module StringSpec
      class TestObject
        def to_str
          "world!"
        end
        
        def ==(other)
          true
        end
      end
    end
    
    ('hello' == StringSpec::TestObject.new).should == true 
    ('world!' == StringSpec::TestObject.new).should == true 
  end
end