require File.dirname(__FILE__) + '/../spec_helper'

describe "The retry statement" do
  it "should raise LocalJumpError if used outside of a block" do
    def bad_meth_retry; retry; end
    should_raise(LocalJumpError) { bad_meth_retry() }
    should_raise(LocalJumpError) { lambda { retry }.call }
  end
  
  it "should retry closest block" do
    exist = [2,3]
    processed = []
    order = []
    [1,2,3,4].each do |x|
      order << x
      begin
        processed << x
        if(exist.include?(x))
          raise StandardError, "included"
        end
      rescue StandardError => e
        exist.delete(x)
        retry
      end
    end
    processed.should == [1,2,2,3,3,4]
    exist.should == []
    order.should == [1,2,3,4]
  end

  # block retry is crazy. rubinius is not planning to currently implement it.
  compliant :mri do
    it "should retry entire enumeration" do
      list = []
      [1,2,3].each do |x|
        list << x
        break if list.size == 6
        retry if x == 3
      end
      list.should == [1,2,3,1,2,3]
    end
  end
end
