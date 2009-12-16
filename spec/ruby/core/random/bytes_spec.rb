require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Random#bytes" do
    it "returns a String" do
      Random.new.bytes(1).should be_an_instance_of(String)
    end

    it "returns a String of the length given as argument" do
      Random.new.bytes(15).length.should == 15
    end

    it "returns an ASCII-8BIT String" do
      Random.new.bytes(15).encoding.should == Encoding::ASCII_8BIT
    end

    it "returns the same output for a given seed" do
      Random.new(33).bytes(2).should == "\x14\\"
    end

    it "returns a random binary String" do
      Random.new.bytes(12).should_not == Random.new.bytes(12)
    end
  end
end
