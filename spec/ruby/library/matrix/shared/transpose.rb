require 'matrix'

describe :matrix_transpose, :shared => true do
  it "returns a transposed matrix" do
    Matrix[[1, 2], [3, 4], [5, 6]].send(@method).should == Matrix[[1, 3, 5], [2, 4, 6]]
  end

  ruby_bug "redmine:1532", "1.8.7" do
    it "can transpose empty matrices" do
      m = Matrix[[], [], []]
      m.send(@method).send(@method).should == m
    end
  end
end
