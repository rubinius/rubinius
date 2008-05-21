require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/atan2'

describe "Math#atan2" do
  it_behaves_like :complex_math_atan2, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan2")
  end
end

describe "Math#atan2!" do
  it_behaves_like :complex_math_atan2_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan2!")
  end
end

describe "Math.atan2" do
  it_behaves_like :complex_math_atan2, Math
end

describe "Math.atan2!" do
  it_behaves_like :complex_math_atan2_bang, Math
end
