require 'matrix'

describe :matrix_rectangular, :shared => true do
  it "returns [receiver.real, receiver.imag]" do
    m = Matrix[ [1.2, Complex(1,2)], [Complex(-2,0.42), 4] ]
    m.send(@method).should == [m.real, m.imag]

    m = Matrix.empty(3, 0)
    m.send(@method).should == [m.real, m.imag]
  end
end
