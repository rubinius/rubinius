require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.8.8" do
  describe "Hash.try_convert" do
    it "returns the argument if passed a Hash" do
      h = {:foo => :glark}
      Hash.try_convert(h).should == h
    end

    it "returns nil if the argument can't be coerced into a Hash" do
      Hash.try_convert(Object.new).should be_nil
    end

    it "does not rescue exceptions raised by #to_hash" do
      obj = mock("to_hash raises")
      obj.should_receive(:to_hash).and_raise(RuntimeError)
      lambda { Hash.try_convert obj }.should raise_error(RuntimeError)
    end

    it "coerces the argument with #to_hash" do
      obj = mock('to_hash')
      obj.should_receive(:to_hash).and_return({:foo => :bar})
      Hash.try_convert(obj).should == {:foo => :bar}
    end

    it "raises a TypeError if #to_hash does not return a Hash" do
      obj = mock("to_hash invalid")
      obj.should_receive(:to_hash).and_return(:invalid_to_hash)
      lambda { Hash.try_convert obj }.should raise_error(TypeError)
    end
  end
end
