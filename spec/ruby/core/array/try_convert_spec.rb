require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Array.try_convert" do
  ruby_version_is "1.8.8" do
    it "returns self for arrays" do
      x = [1,2,3]
      Array.try_convert(x).should equal(x)
    end

    it "converts using :to_ary" do
      arr = ArraySpecs::ArrayConvertable.new(1,2,3)
      Array.try_convert(arr).should == [1,2,3]
      arr.called.should == :to_ary
    end

    it "returns nil when there is no :to_ary" do
      Array.try_convert(-1).should be_nil
    end

    it "does not rescue exceptions" do
      obj = mock("to_ary raises")
      obj.should_receive(:to_ary).and_raise(RuntimeError)
      lambda{ Array.try_convert obj }.should raise_error(RuntimeError)
    end

    it "checks the result of the conversion" do
      obj = mock('to_ary invalid')
      obj.should_receive(:to_ary).and_return(:invalid_to_ary)
      lambda{ Array.try_convert obj }.should raise_error(TypeError)
    end
  end
end
