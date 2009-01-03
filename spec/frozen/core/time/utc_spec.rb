require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/shared/gm'
require File.dirname(__FILE__) + '/shared/gmtime'
require File.dirname(__FILE__) + '/shared/time_params'

describe "Time#utc?" do
  it "returns true if time represents a time in UTC (GMT)" do
    Time.now.utc?.should == false
  end  
end

describe "Time.utc" do
  it_behaves_like(:time_gm, :utc)
  it_behaves_like(:time_params, :utc)
end

describe "Time#utc" do
  it_behaves_like(:time_gmtime, :utc)
end  
