require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require  File.dirname(__FILE__) + '/shared/join'

describe "Array#join" do
  it_behaves_like :array_join, :join

  it "does not separates elements when the passed separator is nil" do
    [1, 2, 3].join(nil).should == '123'
  end

  it "uses $, as the default separator (which defaults to nil)" do
    [1, 2, 3].join.should == '123'
    begin
      old, $, = $,, '-'
      [1, 2, 3].join.should == '1-2-3'
    ensure
      $, = old
    end
  end

  it "does not process the separator if the array is empty" do
    a = []
    sep = Object.new
    a.join(sep).should == ""
  end

  it "raises a TypeError if the passed separator is not a string and does not respond to #to_str" do
    obj = mock('.')
    obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(false)
    lambda { [1, 2].send(@method, obj) }.should raise_error(TypeError)
  end
end
