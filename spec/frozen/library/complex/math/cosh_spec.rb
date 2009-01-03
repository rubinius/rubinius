require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/cosh'

describe "Math#cosh" do
  it_behaves_like :complex_math_cosh, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("cosh")
  end
end

describe "Math#cosh!" do
  it_behaves_like :complex_math_cosh_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("cosh!")
  end
end

describe "Math.cosh" do
  it_behaves_like :complex_math_cosh, :_, Math
end

describe "Math.cosh!" do
  it_behaves_like :complex_math_cosh_bang, :_, Math
end
