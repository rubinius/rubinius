shared :bignum_divide do |cmd|
  describe "Bignum##{cmd}" do
    before(:each) do
      @bignum = BignumHelper.sbm(88)
    end
    
    it "returns self divided by other" do
      @bignum.send(cmd, 4).should be_close(268435478, TOLERANCE)
      @bignum.send(cmd, 16.2).should be_close(66280364.9382716, TOLERANCE)
      @bignum.send(cmd, BignumHelper.sbm(2)).should be_close(1, TOLERANCE)
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      lambda { @bignum.send(cmd, 0) }.should raise_error(ZeroDivisionError)
    end

    it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
      @bignum.send(cmd, 0.0).to_s.should == 'Infinity'
      @bignum.send(cmd, -0.0).to_s.should == '-Infinity'
    end

    it "raises a TypeError when given a non-Integer" do
      lambda {
        (obj = Object.new).should_receive(:to_int).any_number_of_times.and_return(10)
        @bignum.div(obj)
      }.should raise_error(TypeError)
      lambda { @bignum.div("2") }.should raise_error(TypeError)
    end
  end
end
