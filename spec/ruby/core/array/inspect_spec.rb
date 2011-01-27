require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

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

  it "taints the result String if the Array is tainted" do
    a = [1, 2].taint
    a.inspect.tainted?.should be_true
  end

  it "taints the result String if an element is tainted" do
    a = [1, "str".taint, 2]
    a.inspect.tainted?.should be_true
  end

  it "does not taint the result String if the Array is empty" do
    a = []
    a.taint
    a.inspect.tainted?.should be_false
  end

  ruby_version_is "1.9" do
    it "propagates untrust from itself or elements" do
      x = [1, 2]
      x.untrust
      s = x.inspect
      s.untrusted?.should == true

      x = [1, s]
      x.untrust
      s = x.inspect
      s.untrusted?.should == true
    end
  end
end
