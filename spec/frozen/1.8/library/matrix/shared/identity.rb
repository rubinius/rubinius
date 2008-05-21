require 'matrix'

shared :matrix_identity do |cmd|
  describe "Matrix.#{cmd}" do
    it "returns a Matrix" do
      Matrix.send(cmd, 2).class.should == Matrix
    end
    
    it "returns a n x n identity matrix" do
      Matrix.send(cmd, 3).should == Matrix.scalar(3, 1)
      Matrix.send(cmd, 100).should == Matrix.scalar(100, 1)
    end
  end
end
