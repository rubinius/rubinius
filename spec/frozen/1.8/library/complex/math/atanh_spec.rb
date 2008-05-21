require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/atanh'

describe "Math#atanh" do
  it_behaves_like :complex_math_atanh, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("atanh")
  end
end

describe "Math#atanh!" do
  it_behaves_like :complex_math_atanh_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atanh!")
  end
end

describe "Math.atanh" do
  it_behaves_like :complex_math_atanh, Math
end

describe "Math.atanh!" do
  it_behaves_like :complex_math_atanh_bang, Math
end
