code = Class.method_table.lookup(:new).get_method
code.serial = Rubinius::CompiledCode::KernelMethodSerial

class Class
  undef_method :append_features
  undef_method :extend_object
  undef_method :module_function
  undef_method :prepend_features
end
