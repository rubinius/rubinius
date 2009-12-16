require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#frozen?" do
  it "returns true if array is frozen" do
    a = [1, 2, 3]
    a.frozen?.should == false
    a.freeze
    a.frozen?.should == true
  end

  not_compliant_on :rubinius do
    ruby_version_is "" .. "1.9" do
      it "returns true for an array being sorted by #sort!" do
        a = [1, 2, 3]
        a.sort! { |x,y| a.frozen?.should == true; x <=> y }
      end
    end

    ruby_version_is "1.9" do
      it "returns false for an array being sorted by #sort!" do
        a = [1, 2, 3]
        a.sort! { |x,y| a.frozen?.should == false; x <=> y }
      end
    end

    it "returns false for an array being sorted by #sort" do
      a = [1, 2, 3]
      a.sort { |x,y| a.frozen?.should == false; x <=> y }
    end
  end
end
