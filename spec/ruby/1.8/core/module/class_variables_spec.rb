require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#class_variables" do
  it "returns an Array with the names of class variables of self and self's ancestors" do
    ModuleSpecs::ClassVars::A.class_variables.should include("@@a_cvar")
    ModuleSpecs::ClassVars::M.class_variables.should include("@@m_cvar")
    ModuleSpecs::ClassVars::B.class_variables.should include("@@a_cvar", "@@b_cvar", "@@m_cvar")
  end
  
  it "returns an Array with names of class variables defined in metaclasses" do
    ModuleSpecs::CVars.class_variables.should include("@@cls", "@@meta")
  end
end
