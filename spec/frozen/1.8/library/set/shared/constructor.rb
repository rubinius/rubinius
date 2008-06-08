shared :set_constructor do |klass|
  describe "#{klass}.[]" do
    it "returns a new #{klass} populated with the passed Objects" do
      set = klass[1, 2, 3]

      set.instance_of?(klass).should be_true
      set.size.should eql(3)

      set.should include(1)
      set.should include(2)
      set.should include(3)
    end
  end
end