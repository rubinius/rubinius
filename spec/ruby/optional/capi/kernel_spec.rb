require File.expand_path('../spec_helper', __FILE__)

load_extension("kernel")

describe "C-API Kernel function" do
  before :each do
    @s = CApiKernelSpecs.new
  end

  describe "rb_block_given_p" do
    it "returns false if no block is passed" do
      @s.rb_block_given_p.should == false
    end

    it "returns true if a block is passed" do
      (@s.rb_block_given_p { puts "FOO" } ).should == true
    end
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

  describe "rb_rescue" do
    before :each do
      @proc = lambda { |x| x }
      @arg_error_proc = lambda { raise ArgumentError, '' }
      @std_error_proc = lambda { raise StandardError, '' }
      @exc_error_proc = lambda { raise Exception, '' }
    end

    it "executes passed function" do
      @s.rb_rescue(@proc, :no_exc, @proc, :exc).should == :no_exc
    end

    it "executes passed 'raise function' if a StardardError exception is raised" do
      @s.rb_rescue(@arg_error_proc, nil, @proc, :exc).should == :exc
      @s.rb_rescue(@std_error_proc, nil, @proc, :exc).should == :exc
    end

    it "raises an exception if passed function raises an exception other than StardardError" do
      lambda { @s.rb_rescue(@exc_error_proc, nil, @proc, nil) }.should raise_error(Exception)
    end

    it "raises an exception if any exception is raised inside 'raise function'" do
      lambda { @s.rb_rescue(@std_error_proc, nil, @std_error_proc, nil) }.should raise_error(StandardError)
    end

    it "makes $! available only during 'raise function' execution" do
      @s.rb_rescue(@std_error_proc, nil, lambda { $! }, nil).class.should == StandardError
      $!.should == nil
    end
  end

  describe "rb_rescue2" do
    it "only rescues if one of the passed exceptions is raised" do
      proc = lambda { |x| x }
      arg_error_proc = lambda { raise ArgumentError, '' }
      run_error_proc = lambda { raise RuntimeError, '' }
      type_error_proc = lambda { raise TypeError, '' }
      @s.rb_rescue2(arg_error_proc, :no_exc, proc, :exc, ArgumentError, RuntimeError).should == :exc
      @s.rb_rescue2(run_error_proc, :no_exc, proc, :exc, ArgumentError, RuntimeError).should == :exc
      lambda {
        @s.rb_rescue2(type_error_proc, :no_exc, proc, :exc, ArgumentError, RuntimeError)
      }.should raise_error(TypeError)
    end
  end

  describe "rb_ensure" do
    it "executes passed function and returns its value" do
      proc = lambda { |x| x }
      @s.rb_ensure(proc, :proc, proc, :ensure_proc).should == :proc
    end

    it "executes passed 'ensure function' when no exception is raised" do
      foo = nil
      proc = lambda { }
      ensure_proc = lambda { |x| foo = x }
      @s.rb_ensure(proc, nil, ensure_proc, :foo)
      foo.should == :foo
    end

    it "executes passed 'ensure function' when an exception is raised" do
      foo = nil
      raise_proc = lambda { raise '' }
      ensure_proc = lambda { |x| foo = x }
      @s.rb_ensure(raise_proc, nil, ensure_proc, :foo) rescue nil
      foo.should == :foo
    end

    it "raises the same exception raised inside passed function" do
      raise_proc = lambda { raise RuntimeError, 'foo' }
      proc = lambda { }
      lambda { @s.rb_ensure(raise_proc, nil, proc, nil) }.should raise_error(RuntimeError, 'foo')
    end
  end

  describe "rb_eval_string" do
    it "evaluates a string of ruby code" do
      @s.rb_eval_string("1+1").should == 2
    end
  end

  describe "rb_block_proc" do
    it "converts the implicit block into a proc" do
      proc = @s.rb_block_proc() { 1+1 }
      proc.should be_kind_of Proc
      proc.call.should == 2
    end
  end
end
