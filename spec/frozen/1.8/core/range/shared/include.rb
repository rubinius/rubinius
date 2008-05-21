shared :range_include do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other is an element of self" do
      (0..5).send(cmd, 2).should == true
      (-5..5).send(cmd, 0).should == true
      (-1...1).send(cmd, 10.5).should == false
      (-10..-2).send(cmd, -2.5).should == true
      ('C'..'X').send(cmd, 'M').should == true
      ('C'..'X').send(cmd, 'A').should == false
      ('B'...'W').send(cmd, 'W').should == false
      ('B'...'W').send(cmd, 'Q').should == true
      (0xffff..0xfffff).send(cmd, 0xffffd).should == true
      (0xffff..0xfffff).send(cmd, 0xfffd).should == false
      (0.5..2.4).send(cmd, 2).should == true
      (0.5..2.4).send(cmd, 2.5).should == false
      (0.5..2.4).send(cmd, 2.4).should == true
      (0.5...2.4).send(cmd, 2.4).should == false
    end
  end
end
