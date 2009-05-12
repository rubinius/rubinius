require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#minmax" do
  ruby_version_is '1.8.7' do
    before :each do
      @enum = EnumerableSpecs::Numerous.new(6, 4, 5, 10, 8)

      @strs = EnumerableSpecs::Numerous.new("333", "2", "60", "55555", "1010", "111")
    end

    it "min should return the minimum element" do
      @enum.minmax.should == [4, 10]
      @strs.minmax.should == ["1010", "60" ]
    end

    it "return nil when error" do
      EnumerableSpecs::Empty.new.minmax.should == [nil, nil]
      lambda {
        EnumerableSpecs::Numerous.new(Object.new, Object.new).minmax
      }.should raise_error(NoMethodError)
      lambda {
        EnumerableSpecs::Numerous.new(11,"22").minmax
      }.should raise_error(ArgumentError)
      lambda {
        EnumerableSpecs::Numerous.new(11,12,22,33).minmax{|a, b| nil}
      }.should raise_error(ArgumentError)
    end

    it "return the minimun when using a block rule" do
      @enum.minmax {|a,b| b <=> a }.should == [10, 4]
      @strs.minmax {|a,b| a.length <=> b.length }.should == ["2", "55555"]
    end
  end
end
