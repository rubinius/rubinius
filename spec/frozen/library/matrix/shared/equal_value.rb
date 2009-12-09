require 'matrix'

describe :equal, :shared => true do
  before do
    @matrix = Matrix[ [1, 2, 3, 4, 5], [2, 3, 4, 5, 6] ]
  end

  it "returns true for self" do
    @matrix.send(@method, @matrix).should be_true
  end

  it "returns true for equal matrices" do
    @matrix.send(@method, Matrix[ [1, 2, 3, 4, 5], [2, 3, 4, 5, 6] ]).should be_true
  end

  it "returns false for different matrices" do
    @matrix.send(@method, Matrix[ [42, 2, 3, 4, 5], [2, 3, 4, 5, 6] ]).should be_false
    @matrix.send(@method, Matrix[ [1, 2, 3, 4, 5, 6], [2, 3, 4, 5, 6, 7] ]).should be_false
    @matrix.send(@method, Matrix[ [1, 2, 3], [2, 3, 4] ]).should be_false
  end
end
