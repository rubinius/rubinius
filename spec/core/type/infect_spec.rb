require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Type.infect" do
  before do
    @tainted = Object.new.taint
  end

  it "returns the first argument" do
    a = Object.new
    b = Object.new
    Rubinius::Type.infect(a, b).should equal(a)
  end

  it "doesn't taint the first argument if the second is untainted" do
    Rubinius::Type.infect(Object.new, Object.new).tainted?.should be_false
  end

  it "taints the first argument if the second is tainted" do
    Rubinius::Type.infect(Object.new, @tainted).tainted?.should be_true
  end

  it "returns the Fixnum" do
    value = 1
    Rubinius::Type.infect(value,  @tainted).should == value
  end

  it "returns the Bignum" do
    value = bignum_value
    Rubinius::Type.infect(value,  @tainted).should == value
  end

  it "returns the Float" do
    value = 2.0
    Rubinius::Type.infect(value,  @tainted).should == value
  end

  it "returns the Symbol" do
    value = :a
    Rubinius::Type.infect(value,  @tainted).should == value
  end

  it "is a no-op for true" do
    Rubinius::Type.infect(true,  @tainted).tainted?.should be_false
  end

  it "is a no-op for false" do
    Rubinius::Type.infect(false, @tainted).tainted?.should be_false
  end

  it "is a no-op for nil" do
    Rubinius::Type.infect(nil, @tainted).tainted?.should be_false
  end
end
