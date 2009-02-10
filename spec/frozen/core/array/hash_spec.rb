require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#hash" do
  it "returns the same fixnum for arrays with the same content" do
    [].respond_to?(:hash).should == true

    [[], [1, 2, 3]].each do |ary|
      ary.hash.should == ary.dup.hash
      ary.hash.class.should == Fixnum
    end
  end

  ruby_bug "#", "1.8.6.277" do
    it "properly handles recursive arrays" do
      empty = ArraySpecs.empty_recursive_array
      empty.hash.should be_kind_of(Integer)

      array = ArraySpecs.recursive_array
      array.hash.should be_kind_of(Integer)
    end
  end

  #  Too much of an implementation detail? -rue
  not_compliant_on :rubinius do
    it "calls to_int on result of calling hash on each element" do
      ary = Array.new(5) do
        # Can't use should_receive here because it calls hash()
        obj = mock('0')
        def obj.hash()
          def self.to_int() freeze; 0 end
          return self
        end
        obj
      end

      ary.hash
      ary.each { |obj| obj.frozen?.should == true }

      hash = mock('1')
      hash.should_receive(:to_int).and_return(1.hash)

      obj = mock('@hash')
      obj.instance_variable_set(:@hash, hash)
      def obj.hash() @hash end

      [obj].hash.should == [1].hash
    end
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[].hash.should == [].hash
    ArraySpecs::MyArray[1, 2].hash.should == [1, 2].hash
  end

  it "returns same hash code for arrays with the same content" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
  end

  it "returns the same value if arrays are #eql?" do
    a = [1, 2, 3, 4]
    a.fill 'a', 0..3
    b = %w|a a a a|
    a.hash.should == b.hash
    a.should eql(b)
  end
end
