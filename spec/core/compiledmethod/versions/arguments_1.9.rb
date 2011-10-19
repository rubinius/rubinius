require File.expand_path("../../fixtures/arguments_1.9.rb", __FILE__)

describe "CompiledMethod arguments" do
  it "sets up manditory arguments after an optional" do
    CompiledMethodSpecs::MethodOM.total_args.should == 2
    CompiledMethodSpecs::MethodOM.required_args.should == 1
    CompiledMethodSpecs::MethodOM.post_args.should == 1
  end

  it "sets up manditory, optional, manditory" do
    CompiledMethodSpecs::MethodMOM.total_args.should == 3
    CompiledMethodSpecs::MethodMOM.required_args.should == 2
    CompiledMethodSpecs::MethodMOM.post_args.should == 1
  end
end
