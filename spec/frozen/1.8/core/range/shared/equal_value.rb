shared :range_eql do |cmd|
  describe "Range##{cmd}" do
    it "returns true if other has same begin, end, and exclude_end? values" do
      (0..2).send(cmd, 0..2).should == true
      ('G'..'M').send(cmd,'G'..'M').should == true
      (0.5..2.4).send(cmd, 0.5..2.4).should == true
      (5..10).send(cmd, Range.new(5,10)).should == true
      ('D'..'V').send(cmd, Range.new('D','V')).should == true
      (0.5..2.4).send(cmd, Range.new(0.5, 2.4)).should == true
      (0xffff..0xfffff).send(cmd, 0xffff..0xfffff).should == true
      (0xffff..0xfffff).send(cmd, Range.new(0xffff,0xfffff)).should == true
      
      
      ('Q'..'X').send(cmd, 'A'..'C').should == false
      ('Q'...'X').send(cmd, 'Q'..'W').should == false
      ('Q'..'X').send(cmd, 'Q'...'X').should == false
      (0.5..2.4).send(cmd, 0.5...2.4).should == false
      (1482..1911).send(cmd, 1482...1911).should == false
      (0xffff..0xfffff).send(cmd, 0xffff...0xfffff).should == false
    end
    
    it "returns false if other is no Range" do
      (1..10).send(cmd, 1).should == false
      (1..10).send(cmd, 'a').should == false
      (1..10).send(cmd, mock('x')).should == false
    end
  end
end
