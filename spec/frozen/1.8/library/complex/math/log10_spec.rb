require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/log10'

describe "Math#log10" do
  it_behaves_like :complex_math_log10, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log10")
  end
end

describe "Math#log10!" do
  it_behaves_like :complex_math_log10_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log10!")
  end
end

describe "Math.log10" do
  it_behaves_like :complex_math_log10, Math
end

describe "Math.log10!" do
  it_behaves_like :complex_math_log10_bang, Math
end
