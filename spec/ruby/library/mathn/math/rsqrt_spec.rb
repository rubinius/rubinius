require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../shared/rsqrt', __FILE__)

describe "Math#rsqrt" do
  it_behaves_like :mathn_math_rsqrt, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:rsqrt)
  end
end

describe "Math.rsqrt" do
  it_behaves_like :mathn_math_rsqrt, :_, Math
end
