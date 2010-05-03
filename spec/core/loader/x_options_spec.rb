require File.expand_path('../../../spec_helper', __FILE__)

describe "The -Xcompiler.no_rbc option" do
  before :each do
    @single = fixture __FILE__, "single.rb"
    @single_rbc = @single + "c"

    @multi = fixture __FILE__, "multi.rb"
    @multi_rbc = @multi + "c"
  end

  it "prevents writing an .rbc file when running a script" do
    result = ruby_exe @single, :options => "-Xcompiler.no_rbc"
    result.should == ":single\n"
    File.exists?(@single_rbc).should be_false
  end

  it "prevents writing .rbc files when requiring a file" do
    result = ruby_exe @multi, :options => "-Xcompiler.no_rbc"
    result.should == ":single\n:multi\n"
    File.exists?(@single_rbc).should be_false
    File.exists?(@multi_rbc).should be_false
  end
end
