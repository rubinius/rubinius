describe :prime_each, :shared => true do
  before(:all) do
    @enough_seconds = 3
    @primes = [
      2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37,
      41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83,
      89, 97
    ]
  end

  it "iterates the given block over all prime numbers" do
    enumerated = []
    @object.each do |prime|
      break if prime >= 100
      enumerated << prime
    end
    enumerated.should == @primes
  end

  it "infinitely iterates the given block passing a prime" do
    lambda {
      Timeout.timeout(@enough_seconds) {
        @object.each {|prime| 
          primality = (2..Math.sqrt(prime)).all?{|n| prime%n != 0 }
          primality.should be_true
        }
      }
    }.should raise_error(Timeout::Error)
  end

  it "passes a prime to the given block ascendently" do
    prev = 1
    @object.each do |prime|
      break if prime >= 10000
      prime.should > prev
      prev = prime
    end
  end

  it "returns the last evaluated value of the given block" do
    expected = Object.new
    @object.each{ break expected }.should equal(expected)
  end


  it "iterates the given block over all prime numbers which are equal to or less than the first argument" do
    enumerated = []
    @object.each(100) do |prime|
      enumerated << prime
    end
    enumerated.should == @primes
  end

  it "returns the last evaluated value of the given block" do
    expected = Object.new
    @object.each(5){ expected }.should equal(expected)
  end

  it "returns nil when no prime number is enumerated" do
    obj = Object.new
    @object.each(1){ obj }.should equal(nil)
  end

  it "takes a pseudo-prime generator as the second argument" do
    generator = mock('very bad pseudo-prime generator')
    generator.should_receive(:upper_bound=).with(100)
    generator.should_receive(:each).and_yield(2).and_yield(3).and_yield(4).and_yield(5).and_yield(6)

    enumerated = []
    @object.each(100, generator) do |prime| 
      enumerated << prime
    end
    enumerated.should == [2, 3, 4, 5, 6]
  end


  it "returns an enumerator (or compatible object) if no block given" do
    obj = @object.each
    obj.should be_kind_of(Enumerable)
    obj.should respond_to(:with_index)
    obj.should respond_to(:with_object)
    obj.should respond_to(:next)
    obj.should respond_to(:rewind)
  end

  it "returns an enumerator which remembers the given upper bound" do
    enum = @object.each(100)
    enumerated = []
    lambda {
      Timeout.timeout(@enough_seconds) {
        enum.each do |p| 
          enumerated << p
        end
      }
    }.should_not raise_error(Timeout::Error)
    enumerated.should == @primes
  end

  it "returns an enumerator which independently enumerates" do
    enum1 = @object.each(10)
    enum2 = @object.each(10)

    enum1.next.should == 2
    enum1.next.should == 3
    enum1.next.should == 5
    enum2.next.should == 2
    enum1.next.should == 7
    enum2.next.should == 3
  end

  it "returns an enumerator which can be rewinded via #rewind" do
    enum = @object.each(10)

    enum.next.should == 2
    enum.next.should == 3
    enum.rewind
    enum.next.should == 2
  end

  it "starts from 2 regardless of the prior #each" do
    @object.each do |p|
      break p > 10
    end

    prime = nil
    @object.each do |p|
      prime = p
      break
    end
    prime.should == 2
  end

  it "starts from 2 regardless of the prior Prime.each.next" do
    enum = @object.each(100)
    enum.next.should == 2
    enum.next.should == 3

    prime = nil
    @object.each do |p|
      prime = p
      break
    end
    prime.should == 2
  end
end
