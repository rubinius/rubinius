require 'matrix'
require File.dirname(__FILE__) + '/../../../spec_helper'
shared :collect do |cmd|
  describe "Matrix##{cmd}" do
    before :all do
      @data =  [[1,2],[1,2]]
    end
    it "should return a Matrix" do
      Matrix[ *@data ].send(cmd){|n| n * 2 }.class.should == Matrix
    end
    it "should return the right results" do
      Matrix[ *@data ].send(cmd){|n| n * 2 }.should == Matrix[[2,4],[2,4]]
    end
  end
end