require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/log'

describe "Math#log" do
  it_behaves_like :complex_math_log, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:log)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#log!" do
    it_behaves_like :complex_math_log_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:log!)
    end
  end
end

describe "Math.log" do
  it_behaves_like :complex_math_log, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.log!" do
    it_behaves_like :complex_math_log_bang, :_, Math
  end
end
