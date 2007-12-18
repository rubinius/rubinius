shared :array_indexes do |cmd|
  describe "Array##{cmd}" do
    it "returns elements at integer argument indexes (DEPRECATED)" do
      array = [1, 2, 3, 4, 5]

      x = mock('4')
      def x.to_int() 4 end

      params = [1, 0, 5, -1, -8, 10, x]
      array.send(cmd, *params).should == array.values_at(*params)
    end
    
    it "calls to_int on arguments" do
      array = [1, 2, 3, 4, 5]

      x = mock('4')
      def x.to_int() 4 end
      array.send(cmd, x).should == [5]

      x = mock('2')
      x.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      x.should_receive(:method_missing).with(:to_int).and_return(2)
      array.send(cmd, x).should == [3]
    end

    it "returns elements in range arguments as nested arrays (DEPRECATED)" do
      array = [1, 2, 3, 4, 5]
      params = [0..2, 1...3, 4..6]
      array.indexes(*params).should == [[1, 2, 3], [2, 3], [5]]
      array.indices(*params).should == [[1, 2, 3], [2, 3], [5]]
    end
  end
end
