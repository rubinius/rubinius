require File.dirname(__FILE__) + '/../spec_helper'

describe "The super keyword" do
  module Super1
    class A
      def foo(a)
        a << "A#foo"
      end
    end
    class B < A
      def foo(a)
        a << "B#foo"
        super(a)
      end
      def bar(a)
        a << "B#bar"
        foo(a)
      end
    end
  end
  
  it "should call the method above it in inheritance chain" do
    Super1::A.new.foo([]).should == ["A#foo"]
    Super1::B.new.foo([]).should == ["B#foo","A#foo"]
    Super1::B.new.bar([]).should == ["B#bar","B#foo","A#foo"]
  end

  module Super2
    class A
      def foo(a)
        a << "A#foo"
        bar(a)
      end
      def bar(a)
        a << "A#bar"
      end
    end
    class B < A    
      def foo(a)
        a << "B#foo"
        super(a)
      end
      def bar(a)
        a << "B#bar"
        super(a)
      end
    end
  end
  it "should call the method on the calling class" do
    Super2::A.new.foo([]).should == ["A#foo","A#bar"]
    Super2::B.new.foo([]).should == ["B#foo","A#foo","B#bar","A#bar"]
  end

  module Super3
    class A
      def baz(a)
        a << "A#baz"
      end
    end
    class B < A
      def foo(a)
        a << "B#foo"
        baz(a)
      end
    end
    class C < B
      def baz(a)
        a << "C#baz"
        super(a)
      end
    end
  end
  
  it "should search the full inheritence chain" do
    Super3::C.new.baz([]).should == ["C#baz","A#baz"]
    Super3::B.new.foo([]).should == ["B#foo","A#baz"]
    Super3::C.new.foo([]).should == ["B#foo","C#baz","A#baz"]
  end

  module Super4
    class A
      def foo(a)
        a << "A#foo"
      end
      def self.foo(a)
        a << "A::foo"
      end
      def self.bar(a)
        a << "A::bar"
        foo(a)
      end
    end
    class B < A
      def self.foo(a)
        a << "B::foo"
        super(a)
      end
      def self.bar(a)
        a << "B::bar"
        super(a)
      end
    end
  end

  it "should search class methods" do
    Super4::B.new.foo([]).should == ["A#foo"]
    Super4::B.foo([]).should == ["B::foo","A::foo"]
    Super4::B.bar([]).should == ["B::bar","A::bar","B::foo","A::foo"]
  end

  module ModSuper1
    module ModA
      def foo(a)
        a << "ModA#foo"
      end
    end
    class A
      include ModA
    end
    class B < A
      def foo(a)
        a << "B#foo"
        super(a)
      end
      def bar(a)
        a << "B#bar"
        foo(a)
      end
    end
  end
  
  it "should call the method above it in inheritance chain including modules" do
    ModSuper1::A.new.foo([]).should == ["ModA#foo"]
    ModSuper1::B.new.foo([]).should == ["B#foo","ModA#foo"]
    ModSuper1::B.new.bar([]).should == ["B#bar","B#foo","ModA#foo"]
  end

  module ModSuper2
    module ModA
      def foo(a)
        a << "ModA#foo"
        bar(a)
      end      
      def bar(a)
        a << "ModA#bar"
      end
    end
    class A
      include ModA
    end
    module ModB
      def bar(a)
        a << "ModB#bar"
        super(a)
      end      
    end
    class B < A    
      def foo(a)
        a << "B#foo"
        super(a)
      end
      include ModB
    end
  end
  it "should call the method on the calling class including modules" do
    ModSuper2::A.new.foo([]).should == ["ModA#foo","ModA#bar"]
    ModSuper2::B.new.foo([]).should == ["B#foo","ModA#foo","ModB#bar","ModA#bar"]
  end

  module ModSuper3
    class A
      def baz(a)
        a << "A#baz"
      end
    end
    module ModB
      def foo(a)
        a << "ModB#foo"
        baz(a)
      end
    end
    class B < A
      include ModB
    end
    class C < B
      def baz(a)
        a << "C#baz"
        super(a)
      end
    end
  end
  
  it "should search the full inheritence chain including modules" do
    ModSuper3::C.new.baz([]).should == ["C#baz","A#baz"]
    ModSuper3::B.new.foo([]).should == ["ModB#foo","A#baz"]
    ModSuper3::C.new.foo([]).should == ["ModB#foo","C#baz","A#baz"]
  end

  module ModSuper4
    module ModA
      def foo(a)
        a << "ModA::foo"
      end
      def bar(a)
        a << "ModA::bar"
        foo(a)
      end      
    end
    class A
      def foo(a)
        a << "A#foo"
      end
      class << self
        include ModA
      end
    end
    class B < A
      def self.foo(a)
        a << "B::foo"
        super(a)
      end
      def self.bar(a)
        a << "B::bar"
        super(a)
      end
    end
  end

  it "should search class methods including modules" do
    ModSuper4::B.new.foo([]).should == ["A#foo"]
    ModSuper4::B.foo([]).should == ["B::foo","ModA::foo"]
    ModSuper4::B.bar([]).should == ["B::bar","ModA::bar","B::foo","ModA::foo"]
  end
end
