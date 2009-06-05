require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
    
    it "returns nil when there is no :to_ary" do
      String.try_convert(-1).should be_nil
    end
    
    it "should not rescue errors" do
      lambda{
        String.try_convert(StringSpecs::StringNotReallyConvertable.new)
      }.should raise_error
    end
    
    it "checks the result of the conversion" do
      obj = mock('crazy to_str')
      obj.should_receive(:to_str).and_return(:confused)
      lambda{
        String.try_convert(obj)
      }.should raise_error(TypeError)
    end
  end
end