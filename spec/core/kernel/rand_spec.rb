require File.dirname(__FILE__) + '/../../spec_helper'

describe "Kernel.rand" do
  it "produces a vaguely even distribution" do
    # Like several other rand specs, this one is based on probabilities;
    # with a sufficiently high quality RNG, this spec should fail
    # occassionally.

    # Probability of random failure: 1 in 10**20

    counts = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    5000.times do |i|
      n = rand(10)
      counts[n] += 1
    end

    # Under a perfect distribution (which obviously wouldn't be random),
    # all values in 'counts' are 500.
    # We ignore the highest and lowest value, then apply a +/-30%
    # tolerance on the remaining 8 -- this is a trade-off between
    # strictness, and likelihood of random failure.  For comparison, a
    # +/-20% tolerance would increase probability of random failure to
    # about 1 in 10**10, and +/-10% would make the probability
    # approximately 1 in 100.

    counts.sort!
    counts[1].should > 350
    counts[-2].should < 650
  end

  it "can return a bignum when given a large enough limit" do
    # Probability of random failure:
    #   64-bit: 1 in 10**42
    #   32-bit: 1 in 10**139

    values = []
    10.times do
      values << rand(0x12345678901234567890)
    end
    values.max.should be_kind_of(Bignum)
  end

  it "can return a large bignum when given a large enough limit" do
    # Probability of random failure: 1 in 10**80

    values = []
    10.times do
      values << rand(0x123456789012345678901234567890)
    end
    values.max.should > (0x123456789012345678901234567890 / 100_000_000)
  end

  it "can return a fixnum even when given a bignum limit" do
    # Probability of random failure: 1 in 10**60

    small_bignum = 1
    small_bignum *= 2 until small_bignum.is_a? Bignum
    small_bignum *= 2 # for good measure

    values = []
    200.times do
      values << rand(small_bignum)
    end
    values.min.should be_kind_of(Fixnum)
    values.max.should be_kind_of(Bignum)
  end
end

