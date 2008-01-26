require File.dirname(__FILE__) + '/../../spec_helper'

describe "Enumerable#max_by" do
  class EnumMaxBy
    def initialize(num)
      @num = num
    end

    attr_accessor :num

    # Reverse comparison
    def <=>(other)
      other.num <=> @num
    end
  end

  class EnumEmpty
    include Enumerable

    def each()
      # Nothing to do
    end
  end

  # Array is OK here because we know it does not override

  it "requires a block" do
    lambda { [1, 2, 3].max_by }.should raise_error
  end

  it "returns nil if #each yields no objects" do
    EnumEmpty.new.max_by {|o| o.nonesuch }.should == nil
  end

  it "returns the object for whom the value returned by block is the largest" do
    %w[1 2 3].max_by {|obj| obj.to_i }.should == '3'
    %w[three five].max_by {|obj| obj.length }.should == 'three'
  end

  it "returns the object that appears first in #each in case of a tie" do
    # We assume that an Array will yield in order
    a, b, c = '1', '2', '2'
    [a, b, c].max_by {|obj| obj.to_i }.equal?(b).should == true
  end

  it "uses max.<=>(current) to determine order" do
    a, b, c = EnumMaxBy.new(1), EnumMaxBy.new(2), EnumMaxBy.new(3)

    # Just using self here to avoid additional complexity
    [a, b, c].max_by {|obj| obj }.should == a
  end

  it "is able to return the maximum for enums that contain nils" do
    [nil, nil, true].max_by {|o| o.nil? ? 0 : 1 }.should == true
    [nil, nil, true].max_by {|o| o.nil? ? 1 : 0 }.should == nil
  end
end
