require 'matrix'

describe :matrix_transpose, :shared => true do
  it "returns a transposed matrix" do
    Matrix[[1, 2], [3, 4], [5, 6]].send(@method).should == Matrix[[1, 3, 5], [2, 4, 6]]
  end
end
