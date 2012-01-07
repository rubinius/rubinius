# -*- encoding: us-ascii -*-

module FFI::Library
  # Once the kernel is loaded, we want to raise an error if attempting to
  # attach to a non-existent function.
  def ffi_function_missing(name, *args)
    raise FFI::NotFoundError, "Unable to find foreign function '#{name}'"
  end
end
