module KernelSpecs
  class Methods
    def self.ichi; end
    def ni; end
    class << self
      def san; end
    end
  
    private
  
    def self.shi; end
    def juu_shi; end
  
    class << self
      def roku; end

      private
    
      def shichi; end
    end
  
    protected
  
    def self.hachi; end
    def ku; end
  
    class << self
      def juu; end
    
      protected
    
      def juu_ichi; end
    end
  
    public
  
    def self.juu_ni; end
    def juu_san; end
  end

  class A 
    def public_method; :public_method; end
    protected
    def protected_method; :protected_method; end
    private
    def private_method; :private_method; end
  end

  class Binding
    @@super_secret = "password"

    def initialize(n)
      @secret = n
    end
  
    def square(n)
      n * n
    end
    
    def get_binding
      a = true
      @bind = binding

      # Add/Change stuff
      b = true 
      @secret += 1

      @bind
    end
  end
  
  module MethodMissing
    def self.method_missing(*args) :method_missing end
    def self.existing() :existing end

    def self.private_method() :private_method end
    private_class_method :private_method
  end
  
  class MethodMissingC
    def self.method_missing(*args) :method_missing end
    def method_missing(*args) :instance_method_missing end

    def self.existing() :existing end
    def existing() :instance_existing end

    def self.private_method() :private_method end
    def self.protected_method() :protected_method end
    class << self
      private :private_method
      protected :protected_method
    end

    def private_method() :private_instance_method end
    private :private_method

    def protected_method() :protected_instance_method end
    protected :protected_method
  end
  
  module NoMethodMissing
    def self.existing() :existing end

    def self.private_method() :private_method end
    private_class_method :private_method
  end
    
  class NoMethodMissingC
    def self.existing() :existing end
    def existing() :instance_existing end

    def self.private_method() :private_method end
    def self.protected_method() :protected_method end
    class << self
      private :private_method
      protected :protected_method
    end

    def private_method() :private_instance_method end
    private :private_method

    def protected_method() :protected_instance_method end
    protected :protected_method
  end
  
  module BlockGiven
    def self.accept_block
      block_given?
    end
    
    def self.accept_block_as_argument(&block)
      block_given?
    end
  end
end
