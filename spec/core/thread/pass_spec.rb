require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Thread.pass" do
  it "should invoke the scheduler to pass execution to another thread" do
    list = []
    a = Thread.new do
      list << :a; Thread.pass
      list << :c; Thread.pass
      list << :e; Thread.pass
      list << :g
    end
    b = Thread.new do
      list << :b; Thread.pass
      list << :d; Thread.pass
      list << :f; Thread.pass
      list << :h
    end
    a.join
    b.join
    list.should == [:a,:b,:c,:d,:e,:f,:g,:h]
  end
end
