require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.7" do
  require 'securerandom'

  describe "SecureRandom.base64" do
    it "generates a random base64 string out of specified number of random bytes" do
      (16..128).each do |idx|
        base64 = SecureRandom.base64(idx)
        base64.class.should == String
        base64.length.should < 2 * idx
      end
    end

    it "returns an empty string when argument is 0" do
      SecureRandom.base64(0).should == ""
    end

    it "generates different base64 strings with subsequent invocations" do
      # quick and dirty check, but good enough
      values = []
      256.times do
        base64 = SecureRandom.base64
        # make sure the random values are not repeating
        values.include?(base64).should == false
        values << base64
      end
    end

    it "generates a random base64 string out of 32 random bytes" do
      SecureRandom.base64.class.should == String
      SecureRandom.base64.length.should < 32 * 2
    end

    it "treats nil agrument as default one and generates a random base64 string" do
      SecureRandom.base64(nil).class.should == String
      SecureRandom.base64(nil).length.should < 32 * 2
    end

    it "raises ArgumentError on negative arguments" do
      lambda {
        SecureRandom.base64(-1)
      }.should raise_error(ArgumentError)
    end

    it "raises ArgumentError on non-integer arguments" do
      lambda {
        SecureRandom.base64(2.2)
      }.should raise_error(ArgumentError)
    end

    it "raises ArgumentError on non-numeric arguments" do
      lambda {
        SecureRandom.base64(Object.new)
      }.should raise_error(ArgumentError)
    end
  end
end
