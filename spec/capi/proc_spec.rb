require File.expand_path('../spec_helper', __FILE__)

load_extension("proc")

describe "CApiProc" do
  before :each do
    @p = CApiProcSpecs.new
  end

  it "rb_proc_new should return a new valid Proc" do
    my_proc = @p.underline_concat_proc()
    my_proc.kind_of?(Proc).should == true
    my_proc.call('foo', 'bar').should == 'foo_bar'
    my_proc['foo', 'bar'].should == 'foo_bar'
  end

  it "rb_proc_new returned proc should have arity -1" do
    my_proc = @p.underline_concat_proc()
    my_proc.arity.should == -1
  end
end
