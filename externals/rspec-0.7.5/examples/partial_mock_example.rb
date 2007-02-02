require File.dirname(__FILE__) + '/spec_helper'

class MockableClass
  def self.find id
    return :original_return
  end
end

context "A partial mock" do

  specify "should work at the class level" do
    MockableClass.should_receive(:find).with(1).and_return {:stub_return}
    MockableClass.find(1).should_equal :stub_return
  end

  specify "should revert to the original after each spec" do
    MockableClass.find(1).should_equal :original_return
  end

  specify "can be mocked w/ ordering" do
    MockableClass.should_receive(:msg_1).ordered
    MockableClass.should_receive(:msg_2).ordered
    MockableClass.should_receive(:msg_3).ordered
    MockableClass.msg_1
    MockableClass.msg_2
    MockableClass.msg_3
  end
end