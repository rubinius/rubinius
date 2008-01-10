require File.dirname(__FILE__) + '/../spec_helper'

describe "Array literals" do
  it "[] should return a new array populated with the given elements" do
    array = [1, 'a', nil]
    array[0].should == 1
    array[1].should == 'a'
    array[2].should == nil
  end
end

describe "Bareword array literals" do
    
  it "%w() transforms unquoted barewords into an array" do
    a = 3
    %w(a #{3+a} 3).should == ["a", '#{3+a}', "3"]
  end

  it "%W() transforms unquoted barewords into an array, supporting interpolation" do
    a = 3
    %W(a #{3+a} 3).should == ["a", '6', "3"]
  end
end

describe "The unpacking splat operator (*)" do
  it "when applied to a literal nested array, unpacks its elements into the containing array" do
    [1, 2, *[3, 4, 5]].should == [1, 2, 3, 4, 5]
  end

  it "when applied to a nested referenced array, unpacks its elements into the containing array" do
    splatted_array = [3, 4, 5]
    [1, 2, *splatted_array].should == [1, 2, 3, 4, 5]
  end

  it "unpacks the start and count arguments in an array slice assignment" do
    alphabet_1 = ['a'..'z'].to_a
    alphabet_2 = alphabet_1.dup
    start_and_count_args = [1, 10]
    
    alphabet_1[1, 10] = 'a'
    alphabet_2[*start_and_count_args] = 'a'
    
    alphabet_1.should == alphabet_2
  end

  # mSpec doesn't have pending specs yet
  # it "unpacks a literal array into arguments in a method call"
  # 
  # it "unpacks a referenced array into arguments in a method call"
end

describe "The packing splat operator (*)" do
  
end