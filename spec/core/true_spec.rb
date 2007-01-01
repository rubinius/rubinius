require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, to_s, |

context "TrueClass" do
  specify "& should return false if other is nil or false, otherwise true" do
    example do
      p [true & true, true & false, true & nil, true & "", true & Object.new]
    end.should == '[true, false, false, true, true]'
  end

  specify "^ should return true if other is nil or false, otherwise false" do
    example do
      p [true ^ true, true ^ false, true ^ nil, true ^ "", true ^ Object.new]
    end.should == '[false, true, true, false, false]'
  end

  specify "to_s should " do
    example do
    end.should == ''
  end

  specify "! should return " do
    example do
    end.should == ''
  end
end