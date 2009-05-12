require File.dirname(__FILE__) + '/../../spec_helper'
require 'enumerator'

describe "Enumerator.new" do
  it "creates a new custom enumerator with the given object, iterator and arguments" do
    enum = enumerator_class.new(1, :upto, 3)
    enum.should be_kind_of(enumerator_class)
  end

  it "creates a new custom enumerator that responds to #each" do
    enum = enumerator_class.new(1, :upto, 3)
    enum.respond_to?(:each).should == true
  end

  it "creates a new custom enumerator that runs correctly" do
    enumerator_class.new(1, :upto, 3).map{|x|x}.should == [1,2,3]
  end
  
  it "aliases the second argument to :each" do
    enumerator_class.new(1..2).to_a.should == enumerator_class.new(1..2, :each).to_a
  end
  
  it "doesn't check for the presence of the iterator method" do
    enumerator_class.new(nil).should be_kind_of(enumerator_class)
  end

  it "uses the latest define iterator method" do
    class StrangeEach
      def each
        yield :foo
      end
    end
    enum = enumerator_class.new(StrangeEach.new)
    enum.to_a.should == [:foo]
    class StrangeEach
      def each
        yield :bar
      end
    end
    enum.to_a.should == [:bar]
  end

  ruby_version_is ''...'1.9' do
    it "requires an argument" do
      lambda {enumerator_class.new}.should raise_error(ArgumentError)
    end
  end
  ruby_version_is '1.9' do
    it "accepts a block" do
      enum = enumerator_class.new do |yielder|
        yielder.yield 3
        yielder.yield 2
        yielder.yield 1
      end
      enum.should be_kind_of(enumerator_class)
      enum.to_a.should == [3,2,1]
    end

    it "ignores block if arg given" do
      enum = enumerator_class.new([1,2,3]){|y| y.yield 4}
      enum.to_a.should == [1,2,3]
    end
  end    
end
