require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV#each_value" do

  it "returns each value" do
    e = []
    ENV.each_value { |v| e << v }
    env_values = `env`.gsub(/^.*=/,'').split("\n")
    e.size.should == env_values.size
    e.sort == env_values.sort
  end

  it "raises LocalJumpError if no block given" do
    lambda { ENV.each_key }.should raise_error(LocalJumpError)
  end

end
