require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/sqrt'

describe "Math#rsqrt" do
  it_behaves_like :mathn_math_sqrt, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("sqrt")
  end
end

describe "Math.rsqrt" do
  it_behaves_like :mathn_math_sqrt, Math
end
