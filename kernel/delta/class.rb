# -*- encoding: us-ascii -*-

cm = Class.method_table.lookup(:new).method
cm.serial = Rubinius::CompiledCode::KernelMethodSerial

class Class
  undef_method :extend_object
  undef_method :append_features
end
