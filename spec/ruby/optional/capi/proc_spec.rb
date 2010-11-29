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

describe "C-API" do
  before :each do
    @p = CApiProcSpecs.new
  end

  describe "when calling Proc.new from the C-API" do
    it "returns the proc passed to the Ruby method calling into C" do
      prc = @p.rb_Proc_new(false) { :called }
      prc.call.should == :called
    end

    it "returns the proc passed to the Ruby method when multiple rb_funcall's are issued" do
      prc = @p.rb_Proc_new(true) { :called }
      prc.call.should == :called
    end
  end
end
