module FFISpecs::LibraryHelper
  def extended_module(*libs, &block)
    Module.new do
      extend FFI::Library
      ffi_lib(*libs) unless libs.empty?
      module_eval(&block)
    end
  end
end
