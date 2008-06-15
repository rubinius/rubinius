require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_proc')
require File.dirname(__FILE__) + '/ext/subtend_proc'

describe "SubtendProc" do
  before :each do
    @p = SubtendProc.new
  end
  
  it "rb_proc_new should return a new valid Proc" do
    my_proc = @p.underline_concat_proc()
    my_proc.class.should == Proc
    my_proc.call('foo', 'bar').should == 'foo_bar'
    my_proc['foo', 'bar'].should == 'foo_bar'
  end

  it "rb_proc_new returned proc should have arity -1" do
    my_proc = @p.underline_concat_proc()
    my_proc.arity.should == -1
  end
end
