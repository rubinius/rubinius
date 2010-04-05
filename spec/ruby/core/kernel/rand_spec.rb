require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel.rand" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:rand)
  end

  it "returns a float if no argument is passed" do
    rand.should be_kind_of(Float)
  end

  it "returns an integer for an integer argument" do
    rand(77).should be_kind_of(Integer)
  end

  it "returns an integer for a float argument greater than 1" do
    rand(1.3).should be_kind_of(Integer)
  end

  it "returns a float for an argument between -1 and 1" do
    rand(-0.999).should be_kind_of(Float)
    rand(-0.01).should be_kind_of(Float)
    rand(0).should be_kind_of(Float)
    rand(0.01).should be_kind_of(Float)
    rand(0.999).should be_kind_of(Float)
  end

  it "ignores the sign of the argument" do
    [0, 1, 2, 3].should include(rand(-4))
  end

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

  it "never returns a value greater or equal to 1.0 with no arguments" do
    1000.times do
      (0...1.0).should include(rand)
    end
  end

  it "never returns a value greater or equal to any passed in max argument" do
    1000.times do
      (0...100).to_a.should include(rand(100))
    end
  end

  it "calls to_i on its argument" do
    l = mock('limit')
    l.should_receive(:to_i).and_return 7

    rand l
  end
end

describe "Kernel#rand" do
  it "needs to be reviewed for spec completeness"
end
