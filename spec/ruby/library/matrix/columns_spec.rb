require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

describe "Matrix.columns" do
  before :each do
    @a = [1, 2]
    @b = [3, 4]
    @m = Matrix.columns([@a, @b])
  end

  it "creates a Matrix from argument columns" do
    @m.should be_an_instance_of(Matrix)
    @m.column(0).to_a.should == @a
    @m.column(1).to_a.should == @b
  end

  it "accepts Vectors as argument columns" do
    m = Matrix.columns([Vector[*@a], Vector[*@b]])
    m.should == @m
    m.column(0).to_a.should == @a
    m.column(1).to_a.should == @b
  end

  ruby_bug "redmine:1532", "1.8.7" do
    it "handles empty matrices" do
      e = Matrix.columns([])
      e.row_size.should == 0
      e.column_size.should == 0
      e.should == Matrix[]

      v = Matrix.columns([[],[],[]])
      v.row_size.should == 0
      v.column_size.should == 3
      v.should == Matrix[[], [], []].transpose
    end
  end
end
