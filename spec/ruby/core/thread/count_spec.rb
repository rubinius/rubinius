require File.expand_path('../../../spec_helper', __FILE__)

describe "Thread.count" do
  it "returns the number of Threads that would be returned by .list" do
    Thread.count.should == Thread.list.count
  end
end
