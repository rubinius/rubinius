module FFI
  module Library
    def find_type(name)
      @ffi_callbacks ||= {}

      if val = @ffi_callbacks[name]
        return val
      end

      return FFI.find_type(name)
    end
  end
end
