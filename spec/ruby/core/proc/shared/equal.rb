require File.expand_path('../../../../spec_helper', __FILE__)

describe :proc_equal, :shared => true do
  it "returns true if self and other are the same object" do
    p = proc { :foo }
    p.send(@method, p.dup).should be_true

    p = Proc.new { :foo }
    p.send(@method, p.dup).should be_true

    p = lambda { :foo }
    p.send(@method, p.dup).should be_true
  end

  it "returns true if self and other are both procs with the same body" do
    body = proc { :foo }
    p = proc &body
    p2 = proc &body
    p.send(@method, p2).should be_true

    body = lambda { :foo }
    p = proc &body
    p2 = proc &body
    p.send(@method, p2).should be_true
  end

  it "returns true if self and other are both lambdas with the same body" do
    body = proc { :foo }
    p = lambda &body
    p2 = lambda &body
    p.send(@method, p2).should be_true

    body = lambda { :foo }
    p = lambda &body
    p2 = lambda &body
    p.send(@method, p2).should be_true
  end

  it "returns true if self and other are different kinds of procs but have the same body" do
    body = proc { :foo }
    p = lambda &body
    p2 = proc &body
    p.send(@method, p2).should be_true

    body = lambda { :foo }
    p = proc &body
    p2 = lambda &body
    p.send(@method, p2).should be_true
  end

  ruby_version_is "1.9" do
    it "returns true if the bodies of self and other are identical but represented by different objects" do
      foo = proc    { :foo }
      foo2 = lambda { :foo }
      p = lambda &foo
      p2 = proc  &foo2
      p.send(@method, p2).should be_true
    end

    it "returns false if other is not a Proc" do
      p = proc { :foo }
      p.send(@method, []).should be_false

      p = Proc.new { :foo }
      p.send(@method, Object.new).should be_false

      p = lambda { :foo }
      p.send(@method, :foo).should be_false
    end
  end

  it "returns false if self and other are both procs but have different bodies" do
    p = proc { :bar }
    p2 = proc { :foo }
    p.send(@method, p2).should be_false
  end

  it "returns false if self and other are both lambdas but have different bodies" do
    p = lambda { :foo }
    p2 = lambda { :bar }
    p.send(@method, p2).should be_false
  end

  it "returns false if self and other are different kinds of procs and have different bodies" do
    p = lambda { :foo }
    p2 = proc { :bar }
    p.send(@method, p2).should be_false

    p = proc { :foo }
    p2 = lambda { :bar }
    p.send(@method, p2).should be_false
  end
end
