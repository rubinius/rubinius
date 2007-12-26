require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Tuple#shifted" do
    it "returns a tuple with the specified number of elements added to the front" do
      t = Tuple[4, 5, 6].shifted(3)
      t[0].should == nil
      t[1].should == nil
      t[2].should == nil
      t[3].should == 4
      t[4].should == 5
      t[5].should == 6
    end
  end
end
