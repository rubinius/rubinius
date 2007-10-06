require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@array_collect = shared "Array#collect" do |cmd|
  describe "Array##{cmd}" do
    it "returns a copy of array with each element replaced by the value returned by block" do
      a = ['a', 'b', 'c', 'd']
      b = a.send(cmd) { |i| i + '!' }
      b.should == ["a!", "b!", "c!", "d!"]
    end

    it "does not return subclass instance" do
      MyArray[1, 2, 3].send(cmd) { |x| x + 1 }.class.should == Array
    end
  end
end

describe "Array#collect" do
  it_behaves_like(@array_collect, :collect)
end

@array_collect_b = shared "Array#collect!" do |cmd|
  describe "Array##{cmd}" do
    it "replaces each element with the value returned by block" do
      a = [7, 9, 3, 5]
      a.send(cmd) { |i| i - 1 }.equal?(a).should == true
      a.should == [6, 8, 2, 4]
    end

    compliant :mri do
      it "raises TypeError on a frozen array" do
        should_raise(TypeError) { @frozen_array.send(cmd) {} }
      end
    end
  end
end

describe "Array#collect!" do
  it_behaves_like(@array_collect_b, :collect!)
end
