require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/acos'

describe "Math#acos" do
  it_behaves_like :complex_math_acos, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("acos")
  end
end

describe "Math#acos!" do
  it_behaves_like :complex_math_acos_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("acos!")
  end
end

describe "Math.acos" do
  it_behaves_like :complex_math_acos, Math
end

describe "Math.acos!" do
  it_behaves_like :complex_math_acos_bang, Math
end
