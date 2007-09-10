require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@hash_length = shared "Hash#length" do |cmd|
  describe "Hash##{cmd}" do
    it "returns the number of entries" do
      {:a => 1, :b => 'c'}.send(cmd).should == 2
      {:a => 1, :b => 2, :a => 2}.send(cmd).should == 2
      {:a => 1, :b => 1, :c => 1}.send(cmd).should == 3
      {}.send(cmd).should == 0
      Hash.new(5).send(cmd).should == 0
      Hash.new { 5 }.send(cmd).should == 0
    end
  end
end

describe "Hash#length" do
  it_behaves_like(@hash_length, :length)
end
