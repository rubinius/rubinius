require File.dirname(__FILE__) + '/../spec_helper'

# class methods
#  exception, new

# instance methods
#  backtrace, exception, inspect, message, set_backtrace, to_s,
#  to_str, to_yaml

exception_new = shared "Exception.new" do |cmd|
  describe "Exception.#{cmd}" do
    it "creates a new instance of Exception" do
      Exception.should_be_ancestor_of(Exception.send(cmd).class)
    end

    it "sets the message of the Exception when passes a message" do
      Exception.send(cmd, "I'm broken.").message.should == "I'm broken."
    end

    it "returns 'Exception' for message when no message given" do
      Exception.send(cmd).message.should == "Exception"
    end
  end
end

describe "Exception.new" do
  it_behaves_like(exception_new, :new)
end

describe "Exception.exception" do
  it_behaves_like(exception_new, :exception)
end

describe "Exception#message" do
  it "returns the exception message" do
    [Exception.new.message, Exception.new("Ouch!").message].should == ["Exception", "Ouch!"]
  end
  
end

describe "Exception#inspect" do
  it "returns '#<Exception: Exception>' when no message given" do
    Exception.new.inspect.should == "#<Exception: Exception>"
  end
  
  it "includes message when given" do
    [Exception.new("foobar").inspect].should == ["#<Exception: foobar>"]
  end
end

describe "Exception" do
  it "is a Class" do
    Exception.is_a?(Class)
  end

  it "is a superclass of NoMemoryError" do
    Exception.should_be_ancestor_of(NoMemoryError)
  end

  it "is a superclass of ScriptError" do
    Exception.should_be_ancestor_of(ScriptError)
  end
  
  it "is a superclass of SignalException" do
    Exception.should_be_ancestor_of(SignalException)
  end
  
  it "is a superclass of Interrupt" do
    SignalException.should_be_ancestor_of(Interrupt)
  end

  it "is a superclass of StandardError" do
    Exception.should_be_ancestor_of(StandardError)
  end
  
  it "is a superclass of SystemExit" do
    Exception.should_be_ancestor_of(SystemExit)
  end

  it "is a superclass of SystemStackError" do
    Exception.should_be_ancestor_of(SystemStackError)
  end
end

describe "ScriptError" do
  it "is a superclass of LoadError" do
    ScriptError.should_be_ancestor_of(LoadError)
  end

  it "is a superclass of NotImplementedError" do
    ScriptError.should_be_ancestor_of(NotImplementedError)
  end

  it "is a superclass of SyntaxError" do
    ScriptError.should_be_ancestor_of(SyntaxError)
  end
end

describe "StandardError" do
  it "is a superclass of ArgumentError" do
    StandardError.should_be_ancestor_of(ArgumentError)
  end

  it "is a superclass of IOError" do
    StandardError.should_be_ancestor_of(IOError)
  end

  it "is a superclass of IndexError" do
    StandardError.should_be_ancestor_of(IndexError)
  end

  it "is a superclass of LocalJumpError" do
    StandardError.should_be_ancestor_of(LocalJumpError)
  end

  it "is a superclass of NameError" do
    StandardError.should_be_ancestor_of(NameError)
  end
  
  it "is a superclass of RangeError" do
    StandardError.should_be_ancestor_of(RangeError)
  end
  
  it "is a superclass of RegexpError" do
    StandardError.should_be_ancestor_of(RegexpError)
  end

  it "is a superclass of RuntimeError" do
    StandardError.should_be_ancestor_of(RuntimeError)
  end

  it "is a superclass of SecurityError" do
    StandardError.should_be_ancestor_of(SecurityError)
  end

  it "is a superclass of SystemCallError" do
    StandardError.should_be_ancestor_of(SystemCallError.new("").class)
  end
  it "is a superclass of ThreadError" do
    StandardError.should_be_ancestor_of(ThreadError)
  end

  it "is a superclass of TypeError" do
    StandardError.should_be_ancestor_of(TypeError)
  end

  it "is a superclass of ZeroDivisionError" do
    StandardError.should_be_ancestor_of(ZeroDivisionError)
  end
end

describe "SystemCallError.new" do  
  it "requires at least one argumentt" do
    should_raise(ArgumentError) { SystemCallError.new }
  end

  it "takes an optional errno argument" do
    SystemCallError.should_be_ancestor_of(SystemCallError.new("message",1).class)
  end
end

describe "SystemCallError#errno" do
  it "returns nil when no errno given" do
    SystemCallError.new("message").errno.should == nil
  end  
  
  it "returns the errno given as optional argument to new" do
    SystemCallError.new("message", 42).errno.should == 42
  end
end

describe "IOError" do
  it "is a superclass of EOFError" do
    IOError.should_be_ancestor_of(EOFError)
  end
end

describe "NameError" do
  it "is a superclass of NoMethodError" do
    NameError.should_be_ancestor_of(NoMethodError)
  end
end

describe "NameError.new" do
  it "NameError.new should take optional name argument" do
    NameError.new("msg","name").name.should == "name"
  end  
end

describe "RangeError" do
  it "is a superclass of FloatDomainError" do
    RangeError.should_be_ancestor_of(FloatDomainError)
  end
end

describe "NoMethodError.new" do
  it "allows passing method args" do
    NoMethodError.new("msg","name","args").args.should == "args"
  end
end

class NoMethodErrorA; end
class NoMethodErrorB; end

describe "NoMethodError#args" do 
  it "returns an empty array if the caller method had no arguments" do 
    begin
      NoMethodErrorB.new.foo
    rescue Exception => e
      e.args.should == [] 
    end
  end
  
  it "returns an array with the same elements as passed to the method" do 
    begin 
      a = NoMethodErrorA.new
      NoMethodErrorB.new.foo(1,a)
    rescue Exception => e
      e.args.should == [1,a]
      e.args[1].object_id.should == a.object_id
    end
  end
end

class NoMethodErrorC; 
  protected
  def a_protected_method;end
  private 
  def a_private_method; end
end
class NoMethodErrorD; end

describe "NoMethodError#message" do  
  it "for an undefined method match /undefined method/" do 
    begin  
      NoMethodErrorD.new.foo
    rescue Exception => e
      e.class.should == NoMethodError
    end
  end
  
  it "for an protected method match /protected method/" do
    begin
      NoMethodErrorC.new.a_private_method
    rescue Exception => e 
      e.class.should == NoMethodError
    end
  end
  
  it "for private method match /private method/" do 
    begin
      NoMethodErrorC.new.a_protected_method
    rescue Exception => e 
      e.class.should == NoMethodError 
      e.message.match(/protected method/).should_not == nil
    end
  end
end
