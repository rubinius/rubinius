require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@array_length = shared "Array#length" do |cmd|
  describe "Array##{cmd}" do
    it "returns the number of elements" do
      [].send(cmd).should == 0
      [1, 2, 3].send(cmd).should == 3
    end
  end
end

describe "Array#length" do
  it_behaves_like(@array_length, :length)
end
