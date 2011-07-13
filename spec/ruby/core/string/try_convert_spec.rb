require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "String.try_convert" do
  ruby_version_is "1.9" do
    it "returns self for strings" do
      x = "abc"
      String.try_convert(x).should equal(x)
    end

    it "converts using :to_str" do
      str = "hello"
      obj = mock('stringlike')
      obj.should_receive(:to_str).and_return(str)
      String.try_convert(obj).should equal(str)
    end

    it "returns nil when there is no :to_str" do
      String.try_convert(-1).should be_nil
    end

    it "does not rescue exceptions" do
      obj = mock("to_str raises")
      obj.should_receive(:to_str).and_raise(RuntimeError)
      lambda{ String.try_convert obj }.should raise_error(RuntimeError)
    end

    it "checks the result of the conversion" do
      obj = mock('to_str invaled')
      obj.should_receive(:to_str).and_return(:invalid_to_str)
      lambda{ String.try_convert obj }.should raise_error(TypeError)
    end
  end
end
