ruby_version_is "1.9" do

  module BasicObjectSpecs
    class BOSubclass < BasicObject
      def self.kernel_defined?
        defined?(Kernel)
      end

      include ::Kernel
    end
  end

end
