require File.dirname(__FILE__) + '/../complete'

# These specs are copied from RSpec 1.0.8. They should be run with mSpec.

describe "An object where respond_to? is true and does not have method" do
  # When should_receive(:sym) is sent to any object, the Proxy sends
  # respond_to?(:sym) to that object to see if the method should be proxied.
  #
  # If respond_to? itself is proxied, then when the Proxy sends respond_to?
  # to the object, the proxy is invoked and responds yes (if so set in the spec).
  # When the object does NOT actually respond to :sym, an exception is thrown
  # when trying to proxy it.
  #
  # The fix was to keep track of whether :respond_to? had been proxied and, if
  # so, call the munged copy of :respond_to? on the object.

  it "should not raise an exception" do
    obj = Object.new
    obj.should_receive(:respond_to?).with(:foobar).and_return(true)
    obj.should_receive(:foobar).and_return(:baz)
    obj.respond_to?(:foobar).should == true
    obj.foobar.should == :baz
  end

  it "should not raise an exception" do
    obj = mock("obj")
    obj.should_receive(:respond_to?).with(:foobar).and_return(true)
    obj.should_receive(:foobar).and_return(:baz)
    obj.respond_to?(:foobar).should == true
    obj.foobar.should == :baz
  end

end

class LiarLiarPantsOnFire
  def respond_to?(sym)
    true
  end
  
  def self.respond_to?(sym)
    true
  end
end

describe "should_receive" do
  before(:each) do
    @liar = LiarLiarPantsOnFire.new
  end
  
  it "should work when object lies about responding to a method" do
    @liar.should_receive(:something)
    @liar.something
  end

  it "should work when class lies about responding to a method" do
    LiarLiarPantsOnFire.should_receive(:something)
    LiarLiarPantsOnFire.something
  end
  
  it "should cleanup after itself" do
    LiarLiarPantsOnFire.metaclass.instance_methods.should_not include("something")
  end
end
