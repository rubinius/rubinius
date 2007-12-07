require File.dirname(__FILE__) + '/../../spec_helper'

describe "ThreadGroup#add" do
  before(:each) do
    @thread = Thread.new { }
  end
  
  after(:each) do
    @thread.join
  end
  
  it "adds the given thread to a group and returns self" do
    @thread.group.should == nil
    
    tg = ThreadGroup.new
    tg.add(@thread).should == tg
    @thread.group.should == tg
    tg.list.include?(@thread).should == true
  end
  
  it "removes itself from any other threadgroup" do
    tg1 = ThreadGroup.new
    tg2 = ThreadGroup.new
    
    tg1.add(@thread)
    @thread.group.should == tg1
    tg2.add(@thread)
    @thread.group.should == tg2
    tg2.list.include?(@thread).should == true
    tg1.list.include?(@thread).should == false
  end
  
end
