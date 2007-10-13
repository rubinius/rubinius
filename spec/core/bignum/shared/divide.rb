@bignum_divide = shared "Bignum#/" do |cmd|
  describe "Bignum##{cmd}" do
    before(:each) do
      @bignum = BignumHelper.sbm(88)
    end
    
    it "returns self divided by other" do
      @bignum.send(cmd, 4).should_be_close(268435478, TOLERANCE)
      @bignum.send(cmd, 16.2).should_be_close(66280364.9382716, TOLERANCE)
      @bignum.send(cmd, BignumHelper.sbm(2)).should_be_close(1, TOLERANCE)
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { @bignum.send(cmd, 0) }
    end

    it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
      @bignum.send(cmd, 0.0).to_s.should == 'Infinity'
      @bignum.send(cmd, -0.0).to_s.should == '-Infinity'
    end
  end

  it "raises a TypeError when given a non-Integer" do
    should_raise(TypeError) do
      (obj = Object.new).should_receive(:to_int, :count => 0, :returning => 10)
      @bignum.div(obj)
    end
    
    should_raise(TypeError) do
      @bignum.div("2")
    end
  end
end
