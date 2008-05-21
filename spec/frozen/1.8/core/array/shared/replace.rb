shared :array_replace do |cmd|
  describe "Array##{cmd}" do
    it "replaces the elements with elements from other array" do
      a = [1, 2, 3, 4, 5]
      b = ['a', 'b', 'c']
      a.send(cmd, b).should equal(a)
      a.should == b
      a.should_not equal(b)

      a.send(cmd, [4] * 10)
      a.should == [4] * 10

      a.send(cmd, [])
      a.should == []
    end

    it "calls to_ary on its argument" do
      obj = mock('[1,2,3]')
      def obj.to_ary() [1, 2, 3] end

      ary = []
      ary.send(cmd, obj)
      ary.should == [1, 2, 3]

      obj = mock('[]')
      obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_ary).and_return([])

      ary.send(cmd, obj)
      ary.should == []
    end

    it "does not call to_ary on array subclasses" do
      ary = []
      ary.send(cmd, ArraySpecs::ToAryArray[5, 6, 7])
      ary.should == [5, 6, 7]
    end

    compliant_on :ruby, :jruby do
      it "raises a TypeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.send(cmd, ArraySpecs.frozen_array) }.should raise_error(TypeError)
      end
    end
  end
end
