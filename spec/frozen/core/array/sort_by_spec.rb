require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is "1.9.2" do

  describe "Array#sort_by!" do
    it "sorts array in place by passing each element to the given block" do
      a = [1, -2, 3, 9, 1, 5, -5, 1000, -5, 2, -10, 14, 6, 23, 0]
      a.sort_by!{ |e| e.to_s.size }
      a.should == [1, 6, 3, 9, 1, 5, 2, 0, -5, -5, 23, -2, 14, -10, 1000]
    end

    it "returns an Enumerator if not given a block" do
      (1..10).to_a.sort_by!.should be_kind_of(Enumerator)
    end 

    it "completes when supplied a block that always returns the same result" do
      a = [2, 3, 5, 1, 4]
      a.sort_by!{  1 }
      a.class.should == Array
      a.sort_by!{  0 }
      a.class.should == Array
      a.sort_by!{ -1 }
      a.class.should == Array
    end

    ruby_version_is '' ... '1.9' do
      it "raises a TypeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.sort_by! {} }.should raise_error(TypeError)
      end

      it "temporarily freezes self and recovers after sorted" do
        a = [1, 2, 3]
        a.sort_by! { |x,y| a.frozen?.should == true; x <=> y }
        a.frozen?.should == false
      end
    end

    ruby_version_is '1.9' do
      it "raises a RuntimeError on a frozen array" do
        lambda { ArraySpecs.frozen_array.sort_by! {}}.should raise_error(RuntimeError)
      end

    end

    it "returns the specified value when it would break in the given block" do
      [1, 2, 3].sort_by!{ break :a }.should == :a
    end

    it "makes some modification even if finished sorting when it would break in the given block" do
      partially_sorted = (1..5).map{|i|
        ary = [5, 4, 3, 2, 1]
        ary.sort_by!{|x,y| break if x==i; x<=>y}
        ary
      }
      partially_sorted.any?{|ary| ary != [1, 2, 3, 4, 5]}.should be_true
    end
  end
end  
