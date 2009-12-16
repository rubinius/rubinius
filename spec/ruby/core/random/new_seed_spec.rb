require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  describe "Random.new_seed" do
    
    it "returns a Bignum" do
      Random.new_seed.should be_an_instance_of(Bignum)  
    end

    it "returns an arbitrary seed value each time" do
      bigs = 200.times.map { Random.new_seed }
      bigs.uniq.size.should == 200  
    end

    # This is probably implementation-specific, but given the complexity and
    # importance of random number generation, it seems prudent to be explcit
    # about the current behaviour.
    it "returns a number less than 40 digits long" do
      bigs = 2000.times.map { Random.new_seed.to_s.length }.uniq
      bigs.max.should < 40
    end

    it "is not affected by Kernel#srand" do
      begin
        srand 25
        a = Random.new_seed
        srand 25
        b = Random.new_seed
        a.should_not == b
      ensure
        srand Random.new_seed
      end
    end
  end
end
