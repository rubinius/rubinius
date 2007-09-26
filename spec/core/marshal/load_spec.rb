require File.dirname(__FILE__) + '/../../spec_helper'

describe "Marshal.load" do
  it "returns a Ruby Object from conversion of the given serialized data" do
    Marshal.load(Marshal.dump("abc")).should == "abc"
    Marshal.load(Marshal.dump(0..100)).should == (0..100)
    Marshal.load(Marshal.dump(0...100)).should == (0...100)
    Marshal.load(Marshal.dump(:symbol)).should == :symbol
    Marshal.load(Marshal.dump(Object)).should == Object
    Marshal.load(Marshal.dump([[], []])).should == [[], []]
  end
end