require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "CompiledCode#total_args" do
  it "returns the number of arguments including optional and post-optional" do
    CompiledCodeSpecs.executable(:method_om).total_args.should == 2
  end

  it "returns the number of arguments including mandatory, optional, and post-optional" do
    CompiledCodeSpecs.executable(:method_mop).total_args.should == 3
  end
end
