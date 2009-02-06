require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.clear" do
  it "deletes all environment variables" do
    orig = ENV.to_hash
    begin
      ENV.clear
      ENV.should == {}
    ensure
      ENV.replace orig
    end
  end

end
