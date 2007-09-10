require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@hash_value_p = shared "Hash#value?" do |cmd|
  describe "Hash##{cmd}" do
    it "returns true if the value exists in the hash" do
      {:a => :b}.send(cmd, :a).should == false
      {1 => 2}.send(cmd, 2).should == true
      h = Hash.new(5)
      h.send(cmd, 5).should == false
      h = Hash.new { 5 }
      h.send(cmd, 5).should == false
    end

    it "uses == semantics for comparing values" do
      { 5 => 2.0 }.send(cmd, 2).should == true
    end
  end
end

describe "Hash#value?" do
  it_behaves_like(@hash_value_p, :value?)
end

