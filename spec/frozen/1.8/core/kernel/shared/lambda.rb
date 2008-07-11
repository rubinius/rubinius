shared :kernel_lambda do |cmd|
  describe "Kernel.#{cmd}" do
    it "returns a Proc object" do
      send(cmd) { true }.kind_of?(Proc).should == true
    end

    it "raises an ArgumentError when no block is given" do
      lambda { send(cmd) }.should raise_error(ArgumentError)
    end

    it "raises an ArgumentError when given too many arguments" do
      lambda {
        send(cmd) { |a, b| a + b }.call(1, 2, 5)
      }.should raise_error(ArgumentError)
    end

    it "raises an ArgumentError when given too few arguments" do
      lambda {
        send(cmd) { |a, b| a + b }.call(1)
      }.should raise_error(ArgumentError)
    end

    it "returns from block into caller block" do
      # More info in the pickaxe book pg. 359
      def some_method(cmd)
        p = send(cmd) { return 99 }
        res = p.call
        "returned #{res}"
      end

      # Have to pass in the cmd errors otherwise
      some_method(cmd).should == "returned 99"

      def some_method2(&b) b end
      a_proc = send(cmd) { return true }
      res = some_method2(&a_proc)

      res.call.should == true
    end
  end
end
