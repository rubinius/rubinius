shared :fixnum_modulo do |cmd|
  describe "Fixnum##{cmd}" do
    it "returns the modulus obtained from dividing self by the given argument" do
      13.send(cmd, 4).should == 1
      4.send(cmd, 13).should == 4
  
      13.send(cmd, 4.0).should == 1
      4.send(cmd, 13.0).should == 4
  
      1.send(cmd, 2.0).should == 1.0
      200.send(cmd, bignum_value).should == 200
    end

    it "raises a ZeroDivisionError when the given argument is 0" do
      lambda { 13.send(cmd, 0)  }.should raise_error(ZeroDivisionError)
      lambda { 0.send(cmd, 0)   }.should raise_error(ZeroDivisionError)
      lambda { -10.send(cmd, 0) }.should raise_error(ZeroDivisionError)
    end

    it "does not raise a FloatDomainError when the given argument is 0 and a Float" do
      0.send(cmd, 0.0).to_s.should == "NaN" 
      10.send(cmd, 0.0).to_s.should == "NaN" 
      -10.send(cmd, 0.0).to_s.should == "NaN" 
    end

    it "raises a TypeError when given a non-Integer" do
      lambda {
        (obj = mock('10')).should_receive(:to_int).any_number_of_times.and_return(10)
        13.send(cmd, obj)
      }.should raise_error(TypeError)
      lambda { 13.send(cmd, "10")    }.should raise_error(TypeError)
      lambda { 13.send(cmd, :symbol) }.should raise_error(TypeError)
    end
  end
end
