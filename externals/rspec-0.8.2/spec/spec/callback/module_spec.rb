require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "A Module" do
  specify "should provide callback_events and notify_callbacks methods" do
    the_class = Class.new do
      callback_events :foo
    end

    the_obj = the_class.new
    the_obj.foo { :bar }
    the_obj.notify_callbacks(:foo).should == [:bar]
  end

  specify "should support defining multiple callback_events with one call to callback_events" do
    the_class = Class.new do
      callback_events :foo, :bar
    end

    the_obj = the_class.new
    the_obj.foo { 1 }
    the_obj.bar { 2 }
    the_obj.notify_callbacks(:foo).should == [1]
    the_obj.notify_callbacks(:bar).should == [2]
  end

  specify "should provide unregister_callback method" do
    the_class = Class.new do
      callback_events :foo
    end

    the_obj = the_class.new
    bar_proc = proc {:bar}
    the_obj.foo &bar_proc
    the_obj.unregister_callback(:foo, bar_proc)
    the_obj.notify_callbacks(:foo).should == []
  end
end