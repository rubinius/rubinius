require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "A CallbackContainer instance" do
  setup do
    @container = Callback::CallbackContainer.new
  end

  specify "should define and call callbacks" do
    @container.define(:foo) {:bar}
    @container.define(:foo) {:baz}
    @container.notify(:foo).should == [:bar, :baz]
  end

  specify "should undefine callbacks" do
    callback_to_undefine = @container.define(:foo) {:bar}
    @container.define(:foo) {:baz}
    @container.undefine(:foo, callback_to_undefine)
    @container.notify(:foo).should == [:baz]
  end
  
  specify "should clear all callbacks" do
    @container.define(:foo) {:bar}
    @container.define(:baz) {1}
    @container.clear
    @container.notify(:foo).should == []
  end
end
