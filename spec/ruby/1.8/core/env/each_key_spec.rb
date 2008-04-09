require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV#each_key" do

  it "returns each key" do
    e = []
    ENV.each_key { |k| e << k }
    env_keys = `env`.gsub(/=.*$/,'').split("\n")
    e.size.should == env_keys.size
    e.sort == env_keys.sort
  end

  it "raises LocalJumpError if no block given" do
    lambda { ENV.each_key }.should raise_error(LocalJumpError)
  end

end
