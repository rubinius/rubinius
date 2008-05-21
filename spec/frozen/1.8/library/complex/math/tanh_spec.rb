require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/tanh'

describe "Math#tanh" do
  it_behaves_like :complex_math_tanh, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("tanh")
  end
end

describe "Math#tanh!" do
  it_behaves_like :complex_math_tanh_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("tanh!")
  end
end

describe "Math.tanh" do
  it_behaves_like :complex_math_tanh, Math
end

describe "Math.tanh!" do
  it_behaves_like :complex_math_tanh_bang, Math
end
