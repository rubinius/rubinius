shared :bignum_modulo do |cmd|
  describe "Bignum##{cmd}" do
    before(:each) do
      @bignum = bignum_value
    end
    
    it "returns the modulus obtained from dividing self by the given argument" do
      @bignum.send(cmd, 5).should == 3
      @bignum.send(cmd, -5).should == -2
      @bignum.send(cmd, -100).should == -92
      @bignum.send(cmd, 2.22).should be_close(0.780180180180252, TOLERANCE)
      @bignum.send(cmd, bignum_value(10)).should == 9223372036854775808
    end

    it "raises a ZeroDivisionError when the given argument is 0" do
      lambda { @bignum.send(cmd, 0) }.should raise_error(ZeroDivisionError)
      lambda { (-@bignum).send(cmd, 0) }.should raise_error(ZeroDivisionError)
    end

    it "does not raise a FloatDomainError when the given argument is 0 and a Float" do
      @bignum.send(cmd, 0.0).to_s.should == "NaN" 
      (-@bignum).send(cmd, 0.0).to_s.should == "NaN" 
    end

    it "raises a TypeError when given a non-Integer" do
      lambda { @bignum.send(cmd, mock('10')) }.should raise_error(TypeError)
      lambda { @bignum.send(cmd, "10") }.should raise_error(TypeError)
      lambda { @bignum.send(cmd, :symbol) }.should raise_error(TypeError)
    end
  end
end
