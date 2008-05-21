require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/exp'

describe "Math#exp" do
  it_behaves_like :complex_math_exp, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("exp")
  end
end

describe "Math#exp!" do
  it_behaves_like :complex_math_exp_bang, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("exp!")
  end
end

describe "Math.exp" do
  it_behaves_like :complex_math_exp, Math
end

describe "Math.exp!" do
  it_behaves_like :complex_math_exp_bang, Math
end
