require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/atan2'

describe "Math#atan2" do
  it_behaves_like :complex_math_atan2, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan2")
  end
end

describe "Math#atan2!" do
  it_behaves_like :complex_math_atan2_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan2!")
  end
end

describe "Math.atan2" do
  it_behaves_like :complex_math_atan2, :_, Math
end

describe "Math.atan2!" do
  it_behaves_like :complex_math_atan2_bang, :_, Math
end
