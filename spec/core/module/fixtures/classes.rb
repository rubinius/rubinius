module ModuleSpecs
  class Parent
    def public_parent() end
    protected
      def protected_parent() end
    private
      def private_parent() end
  end
    
  module BasicModule
    def public_module() end
    protected
      def protected_module() end
    private
      def private_module() end
  end

  module SuperModule
    failure :rubinius do
      include BasicModule
    end
    
    def public_super_module() end
    protected
      def protected_super_module() end
    private
      def private_super_module() end
  end
    
  class Child < Parent
    failure :rubinius do
      include SuperModule
    end
    
    def public_child() end
    protected
      def protected_child() end
    private
      def private_child() end
  end
end
