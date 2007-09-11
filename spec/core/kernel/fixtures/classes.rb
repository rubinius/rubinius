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
end
