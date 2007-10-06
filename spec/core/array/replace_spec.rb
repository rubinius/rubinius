require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@array_replace = shared "Array#replace" do |cmd|
  describe "Array##{cmd}" do
    it "replaces the elements with elements from other array" do
      a = [1, 2, 3, 4, 5]
      b = ['a', 'b', 'c']
      a.send(cmd, b).equal?(a).should == true
      a.should == b
      a.equal?(b).should == false

      a.send(cmd, [4] * 10)
      a.should == [4] * 10

      a.send(cmd, [])
      a.should == []
    end

    it "calls to_ary on its argument" do
      obj = Object.new
      def obj.to_ary() [1, 2, 3] end

      ary = []
      ary.send(cmd, obj)
      ary.should == [1, 2, 3]

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_ary], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_ary], :returning => [])

      ary.send(cmd, obj)
      ary.should == []
    end

    it "does not call to_ary on array subclasses" do
      ary = []
      ary.send(cmd, ToAryArray[5, 6, 7])
      ary.should == [5, 6, 7]
    end

    compliant :mri do
      it "raises TypeError on a frozen array" do
        should_raise(TypeError) { @frozen_array.send(cmd, @frozen_array) }
      end
    end
  end
end

describe "Array#replace" do
  it_behaves_like(@array_replace, :replace)
end
