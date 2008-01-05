require File.dirname(__FILE__) + '/../../spec_helper'

module ModuleClassVariablesSpec
  class A
    @@a_cvar = :a_cvar
  end

  module M
    @@m_cvar = :m_cvar
  end

  class B < A
    include M

    @@b_cvar = :b_cvar
  end
end

describe "Module#class_variables" do
  it "returns an Array with the names of class variables of self and self's ancestors" do
    ModuleClassVariablesSpec::A.class_variables.should == %w[@@a_cvar]

    ModuleClassVariablesSpec::M.class_variables.should == %w[@@m_cvar]

    ModuleClassVariablesSpec::B.class_variables.sort.should == \
      %w[@@a_cvar @@b_cvar @@m_cvar]
  end
end
