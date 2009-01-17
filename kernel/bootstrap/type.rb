module Type
  # Performs a direct kind_of? check on the object bypassing any method
  # overrides.
  def self.obj_kind_of?(obj, cls)
    Rubinius.asm(cls, obj) {|c, o|
      run c
      run o
      kind_of
    }
  end
end
