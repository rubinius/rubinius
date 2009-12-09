require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.8" do
  
  describe "Hash.try_convert" do
    
    it "returns the argument if passed a Hash" do
      h = {:foo => :glark}
      Hash.try_convert(h).should == h
    end

    it "returns nil if the argument can't be coerced into a Hash" do
      Hash.try_convert(Object.new).should be_nil
    end

    it "coerces the argument with #to_hash" do
      obj = mock('hash')
      obj.should_receive(:to_hash).and_return({:foo => :bar})
      Hash.try_convert(obj).should == {:foo => :bar}
    end

  end
end
