require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures'

# NOTE: A call to define_finalizer does not guarantee that the
# passed proc or callable will be called at any particular time.
# It is highly questionable whether these aspects of ObjectSpace
# should be spec'd at all.
describe "ObjectSpace.define_finalizer" do
  it "raises an ArgumentError if the action does not respond to call" do
    lambda { 
      ObjectSpace.define_finalizer("", 3)
    }.should raise_error(ArgumentError)
  end

  it "accepts an object and a proc" do
    handler = lambda { |obj| obj }
    ObjectSpace.define_finalizer("garbage", handler).should == [0, handler]
  end

  it "accepts an object and a callable" do
    handler = mock("callable")
    def handler.call(obj) end
    ObjectSpace.define_finalizer("garbage", handler).should == [0, handler]
  end

  ruby_bug "#1706", "1.9.2" do
    it "calls finalizer on process termination" do
      rd, wr = IO.pipe
      if Kernel::fork then
        wr.close
        rd.read.should == "finalized"
        rd.close
      else
        rd.close
        handler = Proc.new { wr.write "finalized"; wr.close }
        obj = "Test"
        ObjectSpace.define_finalizer(obj, handler)
        exit 0
      end
    end

    it "doesn't call self-referencing finalizers" do
      rd, wr = IO.pipe
      if Kernel::fork then
        wr.close
        rd.read.should_not == "finalized"
        rd.close
      else
        rd.close
        obj = "Test"
        handler = Proc.new { wr.write "finalized"; wr.close }
        ObjectSpace.define_finalizer(obj, handler)
        exit 0
      end
    end
  end
end
