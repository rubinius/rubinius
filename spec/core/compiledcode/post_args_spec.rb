require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "CompiledCode#post_args" do
  it "returns the number of post-optional arguments" do
    CompiledCodeSpecs.executable(:method_om).post_args.should == 1
    CompiledCodeSpecs.executable(:method_mop).post_args.should == 1
  end
end
