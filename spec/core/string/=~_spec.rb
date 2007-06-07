require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#=~(regexp)" do
  it "should return the position the match starts" do
    ("rudder" =~ /udder/).should == 1
    ("boat" =~ /[^fl]oat/).should == 0
  end
  
  it "should return nil if there is no match" do
    ("bean" =~ /bag/).should == nil
    ("true" =~ /false/).should == nil
  end
end

describe "String#=~(obj)" do
  it "should raise a TypeError if a obj is a string" do
    should_raise(TypeError) do
      "some string" =~ "another string"
    end
  end
  
  it "should invoke obj.=~ with self" do
    module StringSpec
      class TestObject4
        def =~(object)
          true
        end
      end
    end
    
    ("w00t" =~ StringSpec::TestObject4.new).should == true
  end
end