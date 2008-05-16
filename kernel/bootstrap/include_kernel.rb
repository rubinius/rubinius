# depends on: kernel.rb module.rb class.rb

class IncludedModule
  def include_kernel_in_object
    # initialize
    @method_table = Kernel.method_table
    @method_cache = nil
    @name = :Kernel
    @constants = Kernel.constant_table
    @encloser = Kernel.encloser
    @module = Kernel

    # attach
    @superclass = Object.direct_superclass
    Object.superclass = self
  end
end

i = IncludedModule.allocate
i.include_kernel_in_object

