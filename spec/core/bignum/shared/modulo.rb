shared :bignum_modulo do |cmd|
  describe "Bignum##{cmd}" do
    before(:each) do
      @bignum = BignumHelper.sbm
    end
    
    it "returns the modulus obtained from dividing self by the given argument" do
      @bignum.send(cmd, 5).should == 4
      @bignum.send(cmd, -5).should == -1
      @bignum.send(cmd, -100).should == -76
      @bignum.send(cmd, 2.22).should_be_close(0.639999905491734, TOLERANCE)
      @bignum.send(cmd, BignumHelper.sbm(10)).should == 1073741824
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
      lambda {
        (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
        @bignum.send(cmd, obj)
      }.should raise_error(TypeError)
      lambda { @bignum.send(cmd, "10") }.should raise_error(TypeError)
      lambda { @bignum.send(cmd, :symbol) }.should raise_error(TypeError)
    end
  end
end
