require File.expand_path('../../../spec_helper', __FILE__)

describe "Rubinius::Type.infect" do
  it "returns the first argument" do
    a = Object.new
    b = Object.new
    Rubinius::Type.infect(a, b).should equal(a)
  end

  it "doesn't taint the first argument if the second is untainted" do
    Rubinius::Type.infect(Object.new, Object.new).tainted?.should be_false
  end

  it "taints the first argument if the second is tainted" do
    Rubinius::Type.infect(Object.new, Object.new.taint).tainted?.should be_true
  end

  ruby_version_is "1.9" do
    it "doesn't untrust the first argument if the second is trusted" do
      Rubinius::Type.infect(Object.new, Object.new).untrusted?.should be_false
    end

    it "untrusts the first argument if the second is untrusted" do
      Rubinius::Type.infect(Object.new, Object.new.untrust).untrusted?.should be_true
    end
  end

  it "is a no-op for immediate types" do
    Rubinius::Type.infect(1,     Object.new.taint).tainted?.should be_false
    Rubinius::Type.infect(:a,    Object.new.taint).tainted?.should be_false
    Rubinius::Type.infect(true,  Object.new.taint).tainted?.should be_false
    Rubinius::Type.infect(false, Object.new.taint).tainted?.should be_false
    Rubinius::Type.infect(1,     1).tainted?.should be_false
    Rubinius::Type.infect(:a,    :a).tainted?.should be_false
    Rubinius::Type.infect(true,  true).tainted?.should be_false
    Rubinius::Type.infect(false, false).tainted?.should be_false
  end
end
