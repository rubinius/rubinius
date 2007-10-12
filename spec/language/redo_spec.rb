require File.dirname(__FILE__) + '/../spec_helper'

describe "The redo statement" do
  it "raises LocalJumpError if used not within block or while/for loop" do
    def bad_meth; redo; end
    should_raise(LocalJumpError) { bad_meth() }
  end

  it "restarts block execution if used within block" do
    a = []
    lambda {
      a << 1
      redo if a.size < 2
      a << 2
    }.call
    a.should == [1, 1, 2]
  end

  it "should redo closest loop" do
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
        redo
      end
    end
    processed.should == [1,2,2,3,3,4]
    exist.should == []
    order.should == [1,2,2,3,3,4]
  end
end
