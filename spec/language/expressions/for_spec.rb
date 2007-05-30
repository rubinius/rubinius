require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: break, redo, next, retry

# for name[, name]... in expr [do]
#   body
# end
describe "The for expression" do
  it "should iterate over an Enumerable passing each element to the block" do
    j = 0
    for i in 1..3
      j += i
    end
    j.should == 6
  end

  it "should iterate over an Hash passing each key-value pair to the block" do
    k = 0
    l = 0
    
    for i, j in { 1 => 10, 2 => 20 }
      k += i
      l += j
    end
    
    k.should == 3
    l.should == 30
  end

  it "should iterate over any object responding to 'each'" do
    class XYZ
      def each
        (0..10).each { |i| yield i }
      end
    end
    
    j = 0
    for i in XYZ.new
      j += i
    end
    j.should == 55
  end
end