require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, to_s, |

context "FalseClass" do
  specify "& should return false" do
    example do
      p [false & false, false & true, false & nil, false & "", false & Object.new]
    end.should == '[false, false, false, false, false]'
  end

  specify "^ should return false if other is nil or false, otherwise true" do
    example do
      p [false ^ false, false ^ true, false ^ nil, false ^ "", false ^ Object.new]
    end.should == '[false, true, false, true, true]'
  end

  specify "to_s should return the string 'false'" do
    example do
      puts false.to_s
    end.should == 'false'
  end

  specify "| should return false if other is nil or false, otherwise true" do
    example do
      p [false | false, false | true, false | nil, false | "", false | Object.new]
    end.should == '[false, true, false, true, true]'
  end
end
