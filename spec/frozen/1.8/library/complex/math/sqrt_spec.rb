require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/sqrt'

describe "Math#sqrt" do
  it_behaves_like :complex_math_sqrt, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("sqrt")
  end
end

describe "Math#sqrt!" do
  it_behaves_like :complex_math_sqrt_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("sqrt!")
  end
end

describe "Math.sqrt" do
  it_behaves_like :complex_math_sqrt, :_, Math
end

describe "Math.sqrt!" do
  it_behaves_like :complex_math_sqrt_bang, :_, Math
end
