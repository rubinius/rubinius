require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.keys" do

  it "returns all the keys" do
    ENV.keys.sort.should == ENV.to_hash.keys.sort
  end
end
