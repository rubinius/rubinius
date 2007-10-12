@math_log2 = shared "Math.log2" do
  describe "Math.log2" do
    it "returns a float" do
      Math.log2(5.79).should_be_close(2.53356334821451, TOLERANCE)
    end
    
    it "returns the natural logarithm of the argument" do
      Math.log2(1.1).should_be_close(0.137503523749935, TOLERANCE)
      Math.log2(3.14).should_be_close(1.6507645591169, TOLERANCE)
    end

    it "raises an Errno::EDOM if the argument is less than 0" do
      should_raise( Errno::EDOM) { Math.log2(-1e-15) }
    end

    it "raises an ArgumentError if the argument cannot be coerced with Float()" do
      should_raise(ArgumentError) { Math.log2("test") }
    end

    it "raises a TypeError if the argument is nil" do
      should_raise(TypeError) { Math.log2(nil) }
    end

    it "accepts any argument that can be coerced with Float()" do
      Math.log2(MathSpecs::Float.new).should_be_close(0.0, TOLERANCE)
    end
  end
end
