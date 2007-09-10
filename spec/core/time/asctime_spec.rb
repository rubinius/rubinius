require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

@time_asctime = shared "Time#asctime" do |cmd|
  describe "Time##{cmd}" do
    it "returns a canonical string representation of time" do
      t = Time.now
      t.send(cmd).should == t.strftime("%a %b %e %H:%M:%S %Y")
    end
  end
end

describe "Time#asctime" do
  it_behaves_like(@time_asctime, :asctime)
end
