require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#inspect" do
  
  it "returns a stringified representation of the Matrix" do
    Matrix[ [1,2], [2,1] ].inspect.should == "Matrix[[1, 2], [2, 1]]"
  end

  ruby_bug "redmine:1532", "1.8.7" do
    it "returns 'Matrix.empty(...)' for empty matrices" do
      Matrix[ [], [], [] ].inspect.should == "Matrix.empty(3, 0)"
      Matrix.columns([ [], [], [] ]).inspect.should == "Matrix.empty(0, 3)"
    end
  end

  it "calls inspect on its contents" do
    obj = mock("some_value")
    obj.should_receive(:inspect).and_return("some_value")
    Matrix[ [1, 2], [3, obj] ].inspect.should == "Matrix[[1, 2], [3, some_value]]"
  end
end
