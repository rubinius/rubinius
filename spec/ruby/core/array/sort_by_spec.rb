require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "1.9" do

  describe "Array#sort_by!" do
    it "sorts array in place by passing each element to the given block" do
      a = [-100, -2, 1, 200, 30000]
      a.sort_by!{ |e| e.to_s.size }
      a.should == [1, -2, 200, -100, 30000]
    end

    it "returns an Enumerator if not given a block" do
      (1..10).to_a.sort_by!.should be_an_instance_of(enumerator_class)
    end 

    it "completes when supplied a block that always returns the same result" do
      a = [2, 3, 5, 1, 4]
      a.sort_by!{  1 }
      a.should be_kind_of(Array)
      a.sort_by!{  0 }
      a.should be_kind_of(Array)
      a.sort_by!{ -1 }
      a.should be_kind_of(Array)
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
