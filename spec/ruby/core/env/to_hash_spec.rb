require File.expand_path('../../../spec_helper', __FILE__)

describe "ENV.to_hash" do
  it "returns the ENV as a hash" do
    ENV["foo"] = "bar"
    h = ENV.to_hash
    h.is_a?(Hash).should == true
    h["foo"].should == "bar"
    ENV.delete "foo"
  end

  ruby_version_is "1.9" do
    it "uses the locale encoding for keys" do
      ENV.to_hash.keys.all? {|k| k.encoding == Encoding.find('locale') }.should be_true
    end

    it "uses the locale encoding for values" do
      ENV.to_hash.values.all? {|v| v.encoding == Encoding.find('locale') }.should be_true
    end
  end
end
