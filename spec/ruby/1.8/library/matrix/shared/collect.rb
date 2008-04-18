shared :collect do |cmd|
  describe "Matrix##{cmd}" do
    before :all do
      @data = [ [1, 2], [1, 2] ]
    end

    it "returns an instance of Matrix" do
      Matrix[ *@data ].send(cmd){|n| n * 2 }.should be_kind_of(Matrix)
    end

    it "returns a Matrix where each element is the result of the block" do
      Matrix[ *@data ].send(cmd) { |n| n * 2 }.should == Matrix[ [2, 4], [2, 4] ]
    end
  end
end
