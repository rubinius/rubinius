require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#class_variables" do
  it "returns an array with the names of class variables of self and self's ancestors" do
    c = Class.new { class_variable_set(:@@var1, "one") }
    m = Module.new { class_variable_set(:@@var2, "two") }
    c2 = Class.new(c) { include m; class_variable_set(:@@var3, "three") }
    
    Class.new.class_variables.should == []
    Module.new.class_variables.should == []
    c.class_variables.should == ["@@var1"]
    m.class_variables.should == ["@@var2"]
    c2.class_variables.sort.should == ["@@var1", "@@var2", "@@var3"]
  end
end
