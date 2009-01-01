require 'matrix'

describe :matrix_identity, :shared => true do
  it "returns a Matrix" do
    Matrix.send(@method, 2).class.should == Matrix
  end

  it "returns a n x n identity matrix" do
    Matrix.send(@method, 3).should == Matrix.scalar(3, 1)
    Matrix.send(@method, 100).should == Matrix.scalar(100, 1)
  end
end
