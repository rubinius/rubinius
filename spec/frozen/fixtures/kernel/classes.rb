module KernelSpecs
  module MethodMissing
    def self.method_missing(*args) :method_missing end
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
end  
