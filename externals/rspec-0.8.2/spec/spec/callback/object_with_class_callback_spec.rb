dir = File.dirname(__FILE__)
require "#{dir}/../../spec_helper"

context "An Object with class callback" do
  specify "should notify the class callback" do
    klass = Class.new do
      class << self
        callback_events :the_callback
      end
    end

    klass_callback_called = false
    klass.the_callback {klass_callback_called = true}
    obj = klass.new
    obj.send(:notify_class_callbacks, :the_callback)

    klass_callback_called.should == true
  end
end
