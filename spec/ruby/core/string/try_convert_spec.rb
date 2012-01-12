require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "String.try_convert" do
    it "returns the argument if it's a String" do
      x = "abc"
      String.try_convert(x).should equal(x)
    end

    it "returns the argument if it's a kind of String" do
      x = StringSpecs::MyString.new("abc")
      String.try_convert(x).should equal(x)
    end

    it "returns nil when the argument does not respond to #to_str" do
      String.try_convert(-1).should be_nil
    end

    it "sends #to_str to the argument and returns the result if it's nil" do
      obj = mock('stringlike')
      obj.should_receive(:to_str).and_return(nil)
      String.try_convert(obj).should be_nil
    end

    it "sends #to_str to the argument and returns the result if it's a String" do
      str = "hello"
      obj = mock('stringlike')
      obj.should_receive(:to_str).and_return(str)
      String.try_convert(obj).should equal(str)
    end

    it "sends #to_str to the argument and returns the result if it's a kind of String" do
      str = StringSpecs::MyString.new("hello")
      obj = mock('stringlike')
      obj.should_receive(:to_str).and_return(str)
      String.try_convert(obj).should equal(str)
    end

    it "sends #to_str to the argument and raises TypeError if it's not a kind of String" do
      obj = mock("to_str invalid")
      obj.should_receive(:to_str).and_return(:invalid_to_str)
      lambda { String.try_convert obj }.should raise_error(TypeError)
    end

    it "does not rescue exceptions raised by #to_str" do
      obj = mock("to_str raises")
      obj.should_receive(:to_str).and_raise(RuntimeError)
      lambda { String.try_convert obj }.should raise_error(RuntimeError)
    end
  end
end
