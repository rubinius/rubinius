require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.each_value" do

  it "returns each value" do
    e = []
    orig = ENV.to_hash
    begin
      ENV.clear
      ENV["1"] = "3"
      ENV["2"] = "4"
      ENV.each_value { |v| e << v }
      e.should include("3")
      e.should include("4")
    ensure
      ENV.replace orig
    end
  end

  ruby_version_is "" ... "1.8.7" do
    it "raises LocalJumpError if no block given" do
      lambda { ENV.each_value }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator if called without a block" do
      ENV.each_value.should be_kind_of(Enumerable::Enumerator)
    end
  end

end
