require File.dirname(__FILE__) + '/../spec_helper'

module ClassVariableSpec
  class A
    @@a_cvar = :a_cvar
    def a_cvar() @@a_cvar end
    def a_cvar=(val) @@a_cvar = val end
  end

  class B < A; end

  module M
    @@cvar = :value

    def cvar
      @@cvar
    end
  end

  class C
    extend M
  end
end

describe "class variables" do

  # HACK can't compare things directly, wrapping in []
  it "can be accessed from a subclass" do
    [ClassVariableSpec::B.new.a_cvar].should == [:a_cvar]
  end

  it "sets the value in the superclass from the subclass" do
    a = ClassVariableSpec::A.new
    b = ClassVariableSpec::B.new
    b.a_cvar = :new_val

    [a.a_cvar].should == [:new_val]
  end

  it "retrieves the value from the place it is defined" do
    [ClassVariableSpec::C.cvar.should] == [:value]
  end

end

