require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_global')
require File.dirname(__FILE__) + '/ext/subtend_global'

describe "SubtendGlobal" do
  before :each do
    @f = SubtendGlobal.new
  end
  
  it "correctly gets global values" do
    @f.sb_gv_get("$BLAH").should == nil
    @f.sb_gv_get("$SAFE").should == 0
    @f.sb_gv_get("SAFE").should == 0 # rb_gv_get should change SAFE to $SAFE
  end

  it "correctly sets global values" do
    @f.sb_gv_get("$BLAH").should == nil
    @f.sb_gv_set("$BLAH", 10)
    @f.sb_gv_get("$BLAH").should == 10
  end

end
