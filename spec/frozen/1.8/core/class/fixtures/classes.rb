module ClassSpecs
  class Record
    def self.called(sym)
      @called = sym
    end
    
    def self.called?
      @called
    end
  end
  
  module M
    def inherited(klass)
      ::ClassSpecs::Record.called(klass)
      super
    end
  end
  
  class F; end
  class << F
    include M
  end
  
  class A
    def self.inherited(klass)
      ::ClassSpecs::Record.called(klass)
    end
  end
  
  class H < A
    def self.inherited(klass)
      super
    end
  end

  module Inherited
    class A
      SUBCLASSES = []
      def self.inherited(subclass)
        SUBCLASSES << [self, subclass]
      end
    end

    class B < A; end
    class B < A; end # reopen
    class C < B; end
  end
end
