require File.dirname(__FILE__) + '/../spec_helper'

context "Array" do
  specify "'each' should yield each element to the block" do
    rubinius(<<-CODE
        [1,2,3].each { |item| puts item }
      CODE
    ).should == "1\n2\n3"
  end

  specify "'partition' should return two arrays" do
    rubinius(<<-CODE
        puts [].partition.inspect
      CODE
    ).size.should == "[[], []]"
  end
  
  specify "'partition' should return in the left array values for which the block evaluates to true" do
    rubinius(<<-CODE
        puts [0,1,2,3,4,5].partition { |i| [0,2,4].include? i }.inspect
      CODE
    ).should == "[[0, 2, 4], [1, 3, 5]]"
  end
end
