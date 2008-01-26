require File.dirname(__FILE__) + '/../../spec_helper'

describe "Enumerable#min_by" do
  class EnumMinBy
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

  # Array is OK here because we know that it does not override

  it "requires a block" do
    lambda { [1, 2, 3].min_by }.should raise_error
  end

  it "returns nil if #each yields no objects" do
    EnumEmpty.new.min_by {|o| o.nonesuch }.should == nil
  end

  it "returns the object for whom the value returned by block is the smallest" do
    %w[1 2 3].min_by {|obj| obj.to_i }.should == '1'
    %w[three five].min_by {|obj| obj.length }.should == 'five'
  end

  it "returns the object that appears first in #each in case of a tie" do
    # We assume that an Array will yield in order
    a, b, c = '1', '1', '2'
    [a, b, c].min_by {|obj| obj.to_i }.equal?(a).should == true
  end

  it "uses max.<=>(current) to determine order" do
    a, b, c = EnumMinBy.new(1), EnumMinBy.new(2), EnumMinBy.new(3)

    # Just using self here to avoid additional complexity
    [a, b, c].min_by {|obj| obj }.should == c
  end

  it "is able to return the minimum for enums that contain nils" do
    [nil, nil, true].min_by {|o| o.nil? ? 1 : 0 }.should == true
    [nil, nil, true].min_by {|o| o.nil? ? 0 : 1 }.should == nil
  end
end
