require File.expand_path("../../fixtures/arguments_1.9.rb", __FILE__)

describe "CompiledCode arguments" do
  it "sets up manditory arguments after an optional" do
    CompiledCodeSpecs::MethodOM.total_args.should == 2
    CompiledCodeSpecs::MethodOM.required_args.should == 1
    CompiledCodeSpecs::MethodOM.post_args.should == 1
  end

  it "sets up manditory, optional, manditory" do
    CompiledCodeSpecs::MethodMOM.total_args.should == 3
    CompiledCodeSpecs::MethodMOM.required_args.should == 2
    CompiledCodeSpecs::MethodMOM.post_args.should == 1
  end
end
