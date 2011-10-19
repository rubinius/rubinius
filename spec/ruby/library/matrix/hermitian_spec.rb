require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

ruby_version_is "1.9.3" do
  describe "Matrix.hermitian?" do
    it "returns true for a hermitian Matrix" do
      Matrix[[1, 2, Complex(0, 3)], [2, 4, 5], [Complex(0, -3), 5, 6]].hermitian?.should be_true
    end

    it "returns true for a 0x0 empty matrix" do
      Matrix.empty.hermitian?.should be_true
    end

    it "returns false for an assymetric Matrix" do
      Matrix[[1, 2],[-2, 1]].hermitian?.should be_false
    end

    it "raises an error for rectangular matrices" do
      [
        Matrix[[0], [0]],
        Matrix[[0, 0]],
        Matrix.empty(0, 2),
        Matrix.empty(2, 0),
      ].each do |rectangual_matrix|
        lambda {
          rectangual_matrix.hermitian?
        }.should raise_error(Matrix::ErrDimensionMismatch)
      end
    end
  end
end
