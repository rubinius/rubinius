require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#inspect" do
  # FIX: compatibility? --rue
  it "returns a string equivalent to evaluated source code representation" do
    [1, 2, 3].inspect.should == '[1, 2, 3]'
    [1, 2, 3 + 4].inspect.should == '[1, 2, 7]'
    [1, ['a', nil, [], 5.0], [[]], -4].inspect.should == '[1, ["a", nil, [], 5.0], [[]], -4]'
  end

  it "calls inspect on its arguments" do
    items = Array.new(3) do |i|
      obj = mock("#{i}")
      obj.should_receive(:inspect).and_return("items[#{i}]")
      obj
    end
    
    items.inspect.should == '[items[0], items[1], items[2]]'
  end
  
  it "handles recursive arrays" do
    x = [1, 2]
    x << x << 4
    x.inspect.should == '[1, 2, [...], 4]'

    x = [1, 2]
    y = [3, 4]
    x << y
    y << x
    x.inspect.should == '[1, 2, [3, 4, [...]]]'
    y.inspect.should == '[3, 4, [1, 2, [...]]]'
  end
end
