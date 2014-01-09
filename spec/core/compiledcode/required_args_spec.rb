require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "CompiledCode#required_args" do
  it "returns the number of mandatory arguments including only post-optional" do
    CompiledCodeSpecs.executable(:method_om).required_args.should == 1
  end

  it "returns the number of mandatory arguments including pre- and post-optional" do
    CompiledCodeSpecs.executable(:method_mop).required_args.should == 2
  end
end
