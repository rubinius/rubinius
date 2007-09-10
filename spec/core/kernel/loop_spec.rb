require File.dirname(__FILE__) + '/../../spec_helper'

context "Kernel.loop()" do
  specify "loop calls block until it is terminated by a break" do
    i = 0
    loop do
      i += 1
      break if i == 10
    end

    i.should == 10
  end

  specify "loop returns value passed to break" do
    loop do
      break 123
    end.should == 123
  end

  specify "loop returns nil if no value passed to break" do
    loop do
      break
    end.should == nil
  end

  specify "loop raises LocalJumpError if no block given" do
    should_raise(LocalJumpError) { loop }
  end
end
