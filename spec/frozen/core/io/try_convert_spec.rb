require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO.try_convert" do
  ruby_version_is "1.8.8" do
    it "returns self for IO objects" do
      fd_1 = IO.new(1)
      IO.try_convert(fd_1).should equal(fd_1)
    end
    
    it "converts using :to_io" do
      io = File.new(__FILE__)
      obj = mock('ioish')
      obj.should_receive(:to_io).and_return(io)
      IO.try_convert(obj).should equal(io)
    end
    
    it "returns nil when the argument doesn't respond to :to_io" do
      IO.try_convert(-1).should be_nil
    end
    
    it "should not rescue errors" do
      lambda{
        IO.try_convert(IOSpecs::NotConvertible.new)
      }.should raise_error
    end
    
    it "checks the result of the conversion" do
      obj = mock('middle child')
      obj.should_receive(:to_io).and_return(:confused)
      lambda{
        IO.try_convert(obj)
      }.should raise_error(TypeError)
    end
  end
end
