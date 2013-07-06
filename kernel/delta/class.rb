# -*- encoding: us-ascii -*-

code = Class.method_table.lookup(:new).method
code.serial = Rubinius::CompiledCode::KernelMethodSerial

class Class
  undef_method :extend_object
  undef_method :append_features
  undef_method :module_function
end
