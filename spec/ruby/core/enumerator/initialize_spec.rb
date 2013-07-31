# -*- encoding: us-ascii -*-

require File.expand_path('../../../spec_helper', __FILE__)

describe "Enumerator#initialize" do
  before(:each) do
    @uninitialized = enumerator_class.allocate
  end

  it "is a private method" do
    enumerator_class.should have_private_instance_method(:initialize, false)
  end

  it "returns self when given an object" do
    @uninitialized.send(:initialize, Object.new).should equal(@uninitialized)
  end

  ruby_version_is "1.9" do
    it "returns self when given a block" do
      @uninitialized.send(:initialize) {}.should equal(@uninitialized)
    end
  end

  ruby_version_is "1.9.1" do
    # Maybe spec should be broken up?
    it "accepts a block" do
      @uninitialized.send(:initialize) do |yielder|
        r = yielder.yield 3
        yielder << r << 2 << 1
      end
      @uninitialized.should be_an_instance_of(enumerator_class)
      r = []
      @uninitialized.each{|x| r << x; x * 2}
      r.should == [3, 6, 2, 1]
    end

    ruby_version_is "" ... "2.0" do
      it "ignores block if arg given" do
        @uninitialized.send(:initialize, [1,2,3]){|y| y.yield 4}
        @uninitialized.to_a.should == [1,2,3]
      end
    end

    ruby_version_is "2.0" do
      it "needs to be reviewed for spec completeness"
    end
  end
end
