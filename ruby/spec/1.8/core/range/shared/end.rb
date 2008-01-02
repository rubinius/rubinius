shared :range_end do |cmd|
  describe "Range##{cmd}" do
    it "end returns the last element of self" do
      (-1..1).send(cmd).should == 1
      (0..1).send(cmd).should == 1
      ("A".."Q").send(cmd).should == "Q"
      ("A"..."Q").send(cmd).should == "Q"
      (0xffff...0xfffff).send(cmd).should == 1048575
      (0.5..2.4).send(cmd).should == 2.4
    end
  end
end
