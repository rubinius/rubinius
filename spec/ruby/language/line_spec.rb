require File.dirname(__FILE__) + '/../spec_helper'

describe "The __LINE__ constant" do
  it "increments for each line" do    
    cline = __LINE__
    __LINE__.should == cline + 1
    # comment is at cline + 2
    __LINE__.should == cline + 3
  end

  it "is eval aware" do
    eval("__LINE__").should == 1    
    cmd =<<EOF
# comment at line 1
__LINE__
EOF
    eval(cmd).should == 2
  end
end
