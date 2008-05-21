shared :array_collect do |cmd|
  describe "Array##{cmd}" do
    it "returns a copy of array with each element replaced by the value returned by block" do
      a = ['a', 'b', 'c', 'd']
      b = a.send(cmd) { |i| i + '!' }
      b.should == ["a!", "b!", "c!", "d!"]
    end

    it "does not return subclass instance" do
      ArraySpecs::MyArray[1, 2, 3].send(cmd) { |x| x + 1 }.class.should == Array
    end
  end
end

shared :array_collect_b do |cmd|
  describe "Array##{cmd}" do
    it "replaces each element with the value returned by block" do
      a = [7, 9, 3, 5]
      a.send(cmd) { |i| i - 1 }.should equal(a)
      a.should == [6, 8, 2, 4]
    end

    compliant_on :ruby, :jruby do
      it "raises a TypeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.send(cmd) {} }.should raise_error(TypeError)
      end
    end
  end
end
