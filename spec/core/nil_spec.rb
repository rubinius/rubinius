require File.dirname(__FILE__) + '/../spec_helper'

# &, ^, inspect, nil?, to_a, to_f, to_i, to_s, |

context "NilClass" do
  specify "& should return false" do
    example do
      p [nil & nil, nil & true, nil & false, nil & "", nil & Object.new]
    end.should == '[false, false, false, false, false]'
  end

  specify "^ should return false if other is nil or false, otherwise true" do
    example do
      p [nil ^ nil, nil ^ true, nil ^ false, nil ^ "", nil ^ Object.new]
    end.should == '[false, true, false, true, true]'
  end

  specify "inspect should return the string 'nil'" do
    example do
      puts nil.inspect
    end.should == 'nil'
  end

  specify "nil? should return true" do
    example do
      p nil.nil?
    end.should == 'true'
  end

  specify "to_a should return an empty array" do
    example do
      p nil.to_a
    end.should == '[]'
  end

  specify "to_f should return 0.0" do
    example do
      puts nil.to_f
    end.should == '0.0'
  end

  specify "to_i should return 0" do
    example do
      puts nil.to_i
    end.should == '0'
  end

  specify "to_s should return ''" do
    example do
      p nil.to_s
    end.should == '""'
  end

  specify "| should return false if other is nil or false, otherwise true" do
    example do
      p [nil | nil, nil | true, nil | false, nil | "", nil | Object.new]
    end.should == '[false, true, false, true, true]'
  end
end