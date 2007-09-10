require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

@time_to_i = shared "Time#to_i" do |cmd|
  describe "Time##{cmd}" do
    it "returns the value of time as an integer number of seconds since epoch" do
      Time.at(0).send(cmd).should == 0
    end
  end
end

describe "Time#to_i" do
  it_behaves_like(@time_to_i, :to_i)
end
