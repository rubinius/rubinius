require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/tanh'

describe "Math#tanh" do
  it_behaves_like :complex_math_tanh, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:tanh)
  end
end

describe "Math#tanh!" do
  it_behaves_like :complex_math_tanh_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:tanh!)
  end
end

describe "Math.tanh" do
  it_behaves_like :complex_math_tanh, :_, Math
end

describe "Math.tanh!" do
  it_behaves_like :complex_math_tanh_bang, :_, Math
end
