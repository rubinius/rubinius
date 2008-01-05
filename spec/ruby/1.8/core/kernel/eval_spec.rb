require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#eval" do
  it "accepts a Proc object as a binding" do
    x = 1
    bind = proc {}

    eval("x", bind).should == 1
    eval("y = 2", bind).should == 2
    eval("y", bind).should == 2

    eval("z = 3").should == 3
    eval("z", bind).should == 3
  end

  it "does not make Proc locals visible to evaluated code" do
    bind = proc { inner = 4 }
    lambda { eval("inner", bind) }.should raise_error(NameError)
  end

  it "allows a binding to be captured inside an eval" do
    outer_binding = binding
    level1 = eval("binding", outer_binding)
    level2 = eval("binding", level1)
    
    eval("w = 1")
    eval("x = 2", outer_binding)
    eval("y = 3", level1)
    
    eval("w").should == 1
    eval("w", outer_binding).should == 1
    eval("w", level1).should == 1
    eval("w", level2).should == 1
    
    eval("x").should == 2
    eval("x", outer_binding).should == 2
    eval("x", level1).should == 2
    eval("x", level2).should == 2

    eval("y").should == 3
    eval("y", outer_binding).should == 3
    eval("y", level1).should == 3
    eval("y", level2).should == 3
  end

  it "allows Proc and binding to be nested in horrible ways" do
    outer_binding = binding
    proc_binding = eval("proc {l = 5; binding}.call", outer_binding)
    inner_binding = eval("proc {k = 6; binding}.call", proc_binding)

    eval("w = 1")
    eval("x = 2", outer_binding)
    eval("yy = 3", proc_binding)
    eval("z = 4", inner_binding)

    eval("w").should == 1
    eval("w", outer_binding).should == 1
    eval("w", proc_binding).should == 1
    eval("w", inner_binding).should == 1

    eval("x").should == 2
    eval("x", outer_binding).should == 2
    eval("x", proc_binding).should == 2
    eval("x", inner_binding).should == 2

    lambda { eval("yy") }.should raise_error(NameError)
    lambda { eval("yy", outer_binding) }.should raise_error(NameError)
    eval("yy", proc_binding).should == 3
    eval("yy", inner_binding).should == 3

    lambda { eval("z") }.should raise_error(NameError)
    lambda { eval("z", outer_binding) }.should raise_error(NameError)
    lambda { eval("z", proc_binding)  }.should raise_error(NameError)
    eval("z", inner_binding).should == 4

    lambda { eval("l") }.should raise_error(NameError)
    lambda { eval("l", outer_binding) }.should raise_error(NameError)
    eval("l", proc_binding).should == 5
    eval("l", inner_binding).should == 5

    lambda { eval("k") }.should raise_error(NameError)
    lambda { eval("k", outer_binding) }.should raise_error(NameError)
    lambda { eval("k", proc_binding)  }.should raise_error(NameError)
    eval("k", inner_binding).should == 6
  end
end
