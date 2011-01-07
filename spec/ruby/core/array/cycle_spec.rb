describe "Array#cycle" do
  ruby_version_is '1.8.7' do
    it "returns nil and does nothing for non positive n or empty arrays" do
      [1,2,3].cycle(0){ throw "ball"}.should be_nil
      [].cycle(6){ throw "ball"}.should be_nil
    end

    it "cycle as many times as requested" do
      [1,2,3].cycle(2).to_a.should == [1,2,3,1,2,3]
    end

    it "cycles indefinitely if called without argument" do
      bomb = 10
      [1,2,3].cycle do
        bomb -= 1
        break 42 if bomb <= 0
      end.should == 42
      bomb.should == 0
    end

    it "cycles indefinitely if argument is nil" do
      bomb = 10
      [1,2,3].cycle(nil) do
        bomb -= 1
        break 42 if bomb <= 0
      end.should == 42
      bomb.should == 0
    end

    it "doesn't rescue StopIteration" do
      lambda {
        [1,2,3].cycle do
          raise StopIteration
        end
      }.should raise_error(StopIteration)
      lambda {
        [1,2,3].cycle(2) do
          raise StopIteration
        end
      }.should raise_error(StopIteration)
    end

    it "raises a TypeError if passed a non-Numeric and non-nil argument" do
      lambda {
        [1,2,3].cycle("4"){}
      }.should raise_error(TypeError)
      lambda {
        [1,2,3].cycle(false){}
      }.should raise_error(TypeError)
    end

    it "yields successive elements of the array repeatedly" do
      b = []
      [1,2,3].cycle do |elem|
        b << elem
        break if b.size == 7
      end
      b.should == [1,2,3,1,2,3,1]
    end
  end
end
