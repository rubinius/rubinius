require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/log'

describe "Math#log" do
  it_behaves_like :complex_math_log, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log")
  end
end

describe "Math#log!" do
  it_behaves_like :complex_math_log_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log!")
  end
end

describe "Math.log" do
  it_behaves_like :complex_math_log, Math
end

describe "Math.log!" do
  it_behaves_like :complex_math_log_bang, Math
end