require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/log'

describe "Math#log" do
  it_behaves_like :complex_math_log, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log")
  end
end

describe "Math#log!" do
  it_behaves_like :complex_math_log_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("log!")
  end
end

describe "Math.log" do
  it_behaves_like :complex_math_log, :_, Math
end

describe "Math.log!" do
  it_behaves_like :complex_math_log_bang, :_, Math
end