require File.dirname(__FILE__) + '/../../../spec_helper.rb'

class SomethingExpected
  attr_accessor :some_value
end

context "should_not_change" do
  setup do
    @instance = SomethingExpected.new
    @instance.some_value = 5
  end

  specify "should not raise exception when the target is not modified by the block" do
    lambda do
      lambda {nil}.should_not_change(@instance, :some_value)
    end.should_not_raise
  end

  specify "should raise exception when the target is modified by the block" do
    lambda do
      lambda {@instance.some_value = 6}.should_not_change(@instance, :some_value)
    end.should_fail_with "some_value should not have changed, but did change from 5 to 6"
  end
end
