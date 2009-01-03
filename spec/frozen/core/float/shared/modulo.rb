describe :float_modulo, :shared => true do
  it "returns self modulo other" do
    6543.21.send(@method, 137).should be_close(104.21, TOLERANCE)
    5667.19.send(@method, bignum_value).should be_close(5667.19, TOLERANCE)
    6543.21.send(@method, 137.24).should be_close(92.9299999999996, TOLERANCE)

    6543.21.send(@method, 137).should be_close(6543.21.%(137), TOLERANCE)
    5667.19.send(@method, bignum_value).should be_close(5667.19.%(0xffffffff), TOLERANCE)
    6543.21.send(@method, 137.24).should be_close(6543.21.%(137.24), TOLERANCE)
  end

  it "does NOT raise ZeroDivisionError if other is zero" do
    1.0.send(@method, 0).to_s.should == 'NaN'
    1.0.send(@method, 0.0).to_s.should == 'NaN'
  end
end
