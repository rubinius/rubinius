require File.dirname(__FILE__) + '/spec_helper'

load_extension("kernel")

describe "C-API Kernel function" do
  before :each do
    @s = CApiKernelSpecs.new
  end

  describe "rb_raise" do
    it "raises an exception" do
      lambda { @s.rb_raise({}) }.should raise_error(TypeError)
    end

    it "terminates the function at the point it was called" do
      h = {}
      lambda { @s.rb_raise(h) }.should raise_error(TypeError)
      h[:stage].should == :before
    end
  end

  describe "rb_warn" do
    before :each do
      @stderr, $stderr = $stderr, IOStub.new
      @verbose = $VERBOSE
    end

    after :each do
      $stderr = @stderr
      $VERBOSE = @verbose
    end

    it "prints a message to $stderr if $VERBOSE evaluates to true" do
      $VERBOSE = true
      @s.rb_warn("This is a warning")
      $stderr.should =~ /This is a warning/
    end

    it "prints a message to $stderr if $VERBOSE evaluates to false" do
      $VERBOSE = false
      @s.rb_warn("This is a warning")
      $stderr.should =~ /This is a warning/
    end
  end

  describe "rb_sys_fail" do
    it "raises an exception from the value of errno" do
      # If errno = 1 is no EPERM on a platform, we can change the
      # expected exception class to be more generic
      lambda do
        @s.rb_sys_fail("additional info")
      end.should raise_error(Errno::EPERM, /additional info/)
    end
  end

  describe "rb_yield" do
    it "yields passed argument" do
      ret = nil
      @s.rb_yield(1) { |z| ret = z }
      ret.should == 1
    end

    it "returns the result from block evaluation" do
      @s.rb_yield(1) { |z| z * 1000 }.should == 1000
    end

    it "raises LocalJumpError when no block is given" do
      lambda { @s.rb_yield(1) }.should raise_error(LocalJumpError)
    end
  end
end
