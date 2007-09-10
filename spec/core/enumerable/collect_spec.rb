require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

@enumerable_collect = shared "Enumerable#collect" do |cmd|
  describe "Enumerable##{cmd}" do
    before :each do
      @a = EachDefiner.new( 2, 4, 6, 8, 10 )
    end
    
    it "collect should return a new array with the results of passing each element to block" do
      Numerous.new.send(cmd) { |i| i % 2 }.should == [0, 1, 1, 0, 1, 0]
    end 
    
    it "returns true if other has same begin, end, and exclude_end?" do
      #@a.send(cmd).should == [2,4,6,8,10]
      EachDefiner.new().send(cmd) {|a| a }.should == []
      @a.send(cmd) {|a| a-1 }.should == [1,3,5,7,9]
      @a.send(cmd) {|a| 1 }.should == [1,1,1,1,1]
      @a.send(cmd) {|a| [a-1] }.should == [[1],[3],[5],[7],[9]]
    end    
  end
end

describe "Enumerable#collect" do   
  it_behaves_like(@enumerable_collect , :collect) 
end
