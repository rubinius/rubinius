# depends on: kernel.rb module.rb class.rb

class IncludedModule < Module
  ivar_as_index :module => 7

  def include_kernel_in_object
    # initialize
    @methods = Kernel.method_table
    @method_cache = nil
    @name = :Kernel
    @constants = Kernel.constant_table
    @parent = Kernel.parent
    @module = Kernel

    # attach
    @superclass = Object.direct_superclass
    Object.superclass = self
  end
end

i = IncludedModule.allocate
i.include_kernel_in_object

