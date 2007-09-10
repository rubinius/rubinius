require File.dirname(__FILE__) + '/../../spec_helper'

symbol_id2name = shared "Symbol#id2name" do |cmd|
  describe "Symbol\##{cmd}" do
    it "returns the string corresponding to self" do
      :rubinius.send(cmd).should == "rubinius"
      :squash.send(cmd).should == "squash"
      :[].send(cmd).should == "[]"
      :@ruby.send(cmd).should == "@ruby"
      :@@ruby.send(cmd).should == "@@ruby"
    end
  end
end

describe "Symbol#id2name" do
  it_behaves_like(symbol_id2name, :id2name)
end
