require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_constants')
require File.dirname(__FILE__) + '/ext/subtend_constants'

describe "SubtendConstants" do
  before :each do
    @s = SubtendConstants.new
  end

  it "rb_cObject should return the Object class" do
    @s.rb_object.should == Object
  end

  it "rb_mKernel should return the Kernel class" do
    @s.rb_kernel.should == Kernel
  end

  it "rb_eException should return the Exception class" do
    @s.rb_exception.should == Exception
  end

  it "rb_eSystemExit should return the SystemExit class" do
    @s.rb_system_exit.should == SystemExit
  end

  it "rb_eInterrupt should return the Interrupt class" do
    @s.rb_interrupt.should == Interrupt
  end

  it "rb_eSignal should return the SignalException class" do
    @s.rb_signal.should == SignalException
  end

  it "rb_eStandardError should return the StandardError class" do
    @s.rb_standard_error.should == StandardError
  end

  it "rb_eRuntimeError should return the RuntimeError class" do
    @s.rb_runtime_error.should == RuntimeError
  end

  it "rb_eTypeError should return the TypeError class" do
    @s.rb_type_error.should == TypeError
  end

  it "rb_eArgError should return the ArgumentError class" do
    @s.rb_arg_error.should == ArgumentError
  end

  it "rb_eIndexError should return the IndexError class" do
    @s.rb_index_error.should == IndexError
  end

  it "rb_eRangeError should return the RangeError class" do
    @s.rb_range_error.should == RangeError
  end

  it "rb_eNameError should return the NameError class" do
    @s.rb_name_error.should == NameError
  end

  it "rb_eNoMethodError should return the NoMethodError class" do
    @s.rb_no_method_error.should == NoMethodError
  end

  it "rb_eSecurityError should return the SecurityError class" do
    @s.rb_security_error.should == SecurityError
  end

  it "rb_eNotImpError should return the NotImplementedError class" do
    @s.rb_not_imp_error.should == NotImplementedError
  end

  it "rb_eNoMemError should return the NoMemoryError class" do
    @s.rb_no_mem_error.should == NoMemoryError
  end

  it "rb_eScriptError should return the ScriptError class" do
    @s.rb_script_error.should == ScriptError
  end

  it "rb_eSyntaxError should return the SyntaxError class" do
    @s.rb_syntax_error.should == SyntaxError
  end

  it "rb_eLoadError should return the LoadError class" do
    @s.rb_load_error.should == LoadError
  end

  it "rb_eSystemCallError should return the SystemCallError class" do
    @s.rb_system_call_error.should == SystemCallError
  end

  it "rb_mErrno should return the Errno class" do
    @s.rb_errno.should == Errno
  end

  it "rb_eIOError should return the IOError class" do
    @s.rb_io_error.should == IOError
  end

  it "rb_eLocalJumpError should return the LocalJumpError class" do
    @s.rb_local_jump_error.should == LocalJumpError
  end

  it "rb_eEOFError should return the EOFError class" do
    @s.rb_eof_error.should == EOFError
  end

  it "rb_eThreadError should return the ThreadError class" do
    @s.rb_thread_error.should == ThreadError
  end

  it "rb_eZeroDivError should return the ZeroDivisionError class" do
    @s.rb_zero_division_error.should == ZeroDivisionError
  end

  it "rb_eFloatDomainError should return the FloatDomainError class" do
    @s.rb_float_domain_error.should == FloatDomainError
  end

  it "rb_eSysStackError should return the SystemStackError class" do
    @s.rb_sys_stack_error.should == SystemStackError
  end

  it "rb_eRegexpError should return the RegexpError class" do
    @s.rb_regexp_error.should == RegexpError
  end
end

