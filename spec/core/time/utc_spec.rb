require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'
require File.dirname(__FILE__) + '/gm_spec'
require File.dirname(__FILE__) + '/gmtime_spec'

describe "Time#utc?" do
  it "returns true if time represents a time in UTC (GMT)" do
    Time.now.utc?.should == false
  end  
end

describe "Time.utc" do
  it_behaves_like(@time_gm, :utc)
end

describe "Time#utc" do
  it_behaves_like(@time_gmtime, :utc)
end  
