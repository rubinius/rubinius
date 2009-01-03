require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "Exception.exception" do
  it_behaves_like(:exception_new, :exception)
end

describe "Exception" do
  it "is a Class" do
    Exception.should be_kind_of(Class)
  end

  it "is a superclass of NoMemoryError" do
    Exception.should be_ancestor_of(NoMemoryError)
  end

  it "is a superclass of ScriptError" do
    Exception.should be_ancestor_of(ScriptError)
  end
  
  it "is a superclass of SignalException" do
    Exception.should be_ancestor_of(SignalException)
  end
  
  it "is a superclass of Interrupt" do
    SignalException.should be_ancestor_of(Interrupt)
  end

  it "is a superclass of StandardError" do
    Exception.should be_ancestor_of(StandardError)
  end
  
  it "is a superclass of SystemExit" do
    Exception.should be_ancestor_of(SystemExit)
  end

  it "is a superclass of SystemStackError" do
    Exception.should be_ancestor_of(SystemStackError)
  end
end
