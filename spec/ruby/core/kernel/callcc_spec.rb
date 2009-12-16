require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is "1.9" do
  require 'continuation'
end

describe "Kernel#callcc" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:callcc)
  end

  it "is possible to exit a loop like a break" do
    i = 0
    Kernel.callcc do |x|
      loop do
        i += 1
        x.call() if i == 5
      end
    end.should == nil
    i.should == 5
  end

  it "is possible to call a continuation multiple times" do
    i = 0
    cont = nil
    @cont = nil
    Kernel.callcc {|cont| @cont = cont}
    i += 1
    @cont.call() if i < 5
    i.should == 5
  end

  it "returns the results of a block if continuation is not called" do
    cont = nil
    a = callcc {|cont| 0}
    a.should == 0
  end

  it "returns the results of continuation once called" do
    cont = nil
    @cont = nil
    a = callcc {|cont| @cont = cont; 0}
    @cont.call(1) if a == 0
    a.should == 1
  end

  it "returns the arguments to call" do
    callcc {|cont| cont.call }.should == nil
    callcc {|cont| cont.call 1 }.should == 1
    callcc {|cont| cont.call 1,2,3 }.should == [1,2,3]
  end

  it "preserves changes to block-local scope" do
    i = "before"
    cont = callcc { |c| c }
    if cont # nil the second time
      i = "after"
      cont.call
    end
    i.should == "after"
  end

  it "preserves changes to method-local scope" do
    # This spec tests that a continuation shares the same locals
    # tuple as the scope that created it.
    KernelSpecs.before_and_after.should == "after"
  end

  it "raises a LocalJumpError if callcc is not given a block" do
    lambda { Kernel.callcc }.should raise_error(LocalJumpError)
  end
end

describe "Kernel.callcc" do
  it "needs to be reviewed for spec completeness"
end
