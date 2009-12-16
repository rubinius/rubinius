require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#inspect" do
  it "returns a string" do
    [1, 2, 3].inspect.should be_kind_of(String)
  end

  it "calls inspect on its elements" do
    items = Array.new(3) do |i|
      obj = mock("#{i}")
      obj.should_receive(:inspect).and_return("items[#{i}]")
      obj
    end
    str = items.inspect
    str.should include('items[0]')
    str.should include('items[1]')
    str.should include('items[2]')
  end
  
  it "handles recursive arrays" do
    x = [1, 2]
    x << x << 4
    lambda{ x.inspect }.should_not raise_error

    x = [1, 2]
    y = [3, 4]
    x << y
    y << x
    lambda{ x.inspect }.should_not raise_error
    lambda{ y.inspect }.should_not raise_error
  end
end
