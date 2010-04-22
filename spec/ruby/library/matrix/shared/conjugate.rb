require 'matrix'

describe :matrix_conjugate, :shared => true do
  it "returns a matrix with all entries 'conjugated'" do
    Matrix[ [1,   2], [3, 4] ].send(@method).should == Matrix[ [1,   2], [3, 4] ]
    Matrix[ [1.9, Complex(1,1)], [3, 4] ].send(@method).should == Matrix[ [1.9, Complex(1,-1)], [3, 4] ]
  end

  it "returns empty matrices on the same size if empty" do
    Matrix.empty(0, 3).send(@method).should == Matrix.empty(0, 3)
    Matrix.empty(3, 0).send(@method).should == Matrix.empty(3, 0)
  end
end
