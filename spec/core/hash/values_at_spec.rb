require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

shared :hash_values_at do |cmd|
  describe "Hash##{cmd}" do
    it "returns an array of values for the given keys" do
      h = {:a => 9, :b => 'a', :c => -10, :d => nil}
      h.send(cmd).class.should == Array
      h.send(cmd).should == []
      h.send(cmd, :a, :d, :b).class.should == Array
      h.send(cmd, :a, :d, :b).should == [9, nil, 'a']
    end
  end
end

describe "Hash#values_at" do
  it_behaves_like(:hash_values_at, :values_at)
end