# depends on: kernel.rb module.rb class.rb

class IncludedModule < Module
  self.instance_fields = 8

  # HACK: make this a VM exported constant
  self.object_type = 7

  ivar_as_index :__ivars__ => 0, :method_table => 1, :method_cache => 2, :name => 3, :constants => 4, :encloser => 5, :superclass => 6, :module => 7

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

