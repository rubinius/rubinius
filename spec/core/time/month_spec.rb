require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

@time_month = shared "Time#month" do |cmd|
  describe "Time##{cmd}" do
    it "returns the month of the year" do
      Time.at(99999).mon.should == 1
    end
  end
end

describe "Time#month" do
  it_behaves_like(@time_month, :month)
end
