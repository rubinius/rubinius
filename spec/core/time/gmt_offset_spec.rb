require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

@time_gmt_offset = shared "Time#gmt_offset" do |cmd|
  describe "Time##{cmd}" do
    it "returns the offset in seconds between the timezone of time and UTC" do
      with_timezone("AST", 3) do
        Time.new.send(cmd).should == 10800
      end
    end
  end
end

describe "Time#gmt_offset" do
  it_behaves_like(@time_gmt_offset, :gmt_offset)
end
