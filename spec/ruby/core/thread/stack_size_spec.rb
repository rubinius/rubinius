require File.expand_path('../../../spec_helper', __FILE__)

describe "Thread#stack_size" do
  it "returns the Thread's stack size" do
    t = Thread.new { }
    t.stack_size.should be_an_instance_of(Fixnum)
  end
end
