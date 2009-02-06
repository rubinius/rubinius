require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/sqrt'

describe "Math#rsqrt" do
  it_behaves_like :mathn_math_sqrt, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:sqrt)
  end
end

describe "Math.rsqrt" do
  it_behaves_like :mathn_math_sqrt, :_, Math
end
