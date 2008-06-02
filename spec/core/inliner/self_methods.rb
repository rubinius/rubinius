require 'pp' # not sure why i have to do this...

describe Rubinius::Inliner::SimpleSelfCallInliner do
  before :all do
    @inliner = Rubinius::Inliner::SimpleSelfCallInliner.new
  end

  def find_send_ip cm, method
    op = cm.decode.find { |i| i.opcode.to_s =~ /^send/ && i.args.first.is_a?(SendSite) && i.args.first.name == method }
    raise "no send to #{method.inspect} found" unless op
    op.ip
  end

  it "inlines trivial methods" do
    rm = def r; 3 end # omg def returns the compiledmethod. i love you rubinius.
    sm = def s; r + 4 end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods with arguments" do
    rm = def r x; x + 1 end
    sm = def s; r(10) + 4 end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods with locals" do
    rm = def r
      a = 5
      b = 22
      c = a + b
    end

    sm = def s
      a = 10
      b = 22
      d = r + a + b
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods with literals" do
    rm = def r
      a = "haile"
      b = "selassie"
      c = a + " " + b
    end

    sm = def s
      a = "emperor"
      a + " " + r
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods with local jumps" do
    rm = def r x
      y = 5
      if x > 0
        y += 3
      else
        y -= 4
      end
      y
    end

    sm = def s
      2 + r(12)
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods with returns" do
    rm = def r
      1
      return 2
      3
    end

    sm = def s
      4
      r
      5
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == 5
  end

  it "inlines somewhat complicated methods" do
    rm = def r a, b
      i = 0
      j = 0
      while i < b
        j += a
        i += 1
        return j if j > 1000
      end
      j
    end

    sm = def s
      i = 0
      j = 0
      while i < 10
        j += r(i, j)
        i += 1
      end
      j
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines methods that throw exceptions" do
    rm = def r a, b
      raise ArgumentError, "a < b" if a < b
      a + b
    end

    sm = def s
      r 1, 2
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    lambda { s }.should raise_error(ArgumentError)
    lambda { im.activate(self, self.class, []) }.should raise_error(ArgumentError)
  end


  it "inlines methods into methods that throw and catch exceptions" do
    rm = def r
      5
    end

    sm = def s
      r
      raise ArgumentError
      "boo"
    rescue ArgumentError
      "yay"
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    lambda { im.activate(self, self.class, []) }.should_not raise_error(ArgumentError)
    im.activate(self, self.class, []).should == "yay"
  end

  it "inlines methods that throw and catch exceptions" do
    rm = def r
      raise ArgumentError
      3
    rescue ArgumentError
      4
    end

    sm = def s
      5 + r
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    lambda { im.activate(self, self.class, []) }.should_not raise_error(ArgumentError)
    im.activate(self, self.class, []).should == 9
  end

  it "inlines methods that throw exceptions into methods that catch them" do
    rm = def r
      raise ArgumentError
    end

    sm = def s
      r
      "bad"
    rescue ArgumentError => e
      "fine"
    end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)

    #puts "Inlined:::"
    #puts im.decode
    #puts

    s.should == "fine"
    lambda { im.activate(self, self.class, []) }.should_not raise_error(ArgumentError)
  end

  it "inlines methods defined in a new class" do
    rm = class C; def r; 3 end end
    sm = class C; def s; r + 4 end end

    c = C.new
    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(c, C, []).should == c.s()
  end

  it "refuses to inline non-self methods" do
    rm = def r; 3 end
    sm = def s o; o.r end

    lambda { im = @inliner.inline sm, rm, find_send_ip(sm, :r) }.
      should raise_error(Rubinius::Inliner::Rejection)
  end

  it "inlines getters on self (hell yes)" do
    class C
      def initialize a; @a = a end
      attr_reader :a
      def s; a + 5 end
    end

    rm = C.instance_method(:a).compiled_method
    sm = C.instance_method(:s).compiled_method

    c = C.new 3
    im = @inliner.inline sm, rm, find_send_ip(sm, :a)
    im.activate(c, C, []).should == c.s()
  end

  it "inlines setters on self" do
    class C
      def initialize a; @a = a end
      attr_accessor :a
      def s; self.a = 5 end
    end

    rm = C.instance_method(:a=).compiled_method
    sm = C.instance_method(:s).compiled_method

    c = C.new 3
    im = @inliner.inline sm, rm, find_send_ip(sm, :a=)
    im.activate(c, C, []).should == 5
    c.a.should == 5
  end

  it "inlines delegated methods" do
    define_method(:r) { 3 }
    rm = instance_method(:r).compiled_method
    sm = def s; r + 4 end

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines into delegated methods" do
    rm = def r; 3 end
    define_method(:s) { r + 4 }
    sm = instance_method(:s).compiled_method

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines delegated methods into delegated methods" do
    define_method(:r) { 3 }
    define_method(:s) { r + 4 }
    rm = instance_method(:r).compiled_method
    sm = instance_method(:s).compiled_method

    im = @inliner.inline sm, rm, find_send_ip(sm, :r)
    im.activate(self, self.class, []).should == s()
  end

  it "inlines in a nestable manner" do
    am = def a; 5 end
    bm = def b; a + 4 end
    cm = def c; b + 3 end
    dm = def d; c + 2 end
    em = def e; d + 1 end

    im = @inliner.inline em, dm, find_send_ip(em, :d)
    im = @inliner.inline im, cm, find_send_ip(im, :c)
    im = @inliner.inline im, bm, find_send_ip(im, :b)
    im = @inliner.inline im, am, find_send_ip(im, :a)

    im.activate(self, self.class, []).should == e()

    ## now the other way
    im = @inliner.inline bm, am, find_send_ip(bm, :a)
    im = @inliner.inline cm, im, find_send_ip(cm, :b)
    im = @inliner.inline dm, im, find_send_ip(dm, :c)
    im = @inliner.inline em, im, find_send_ip(em, :d)

    im.activate(self, self.class, []).should == e()
  end
end
