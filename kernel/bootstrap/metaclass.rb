class MetaClass
  ivar_as_index :__ivars__ => 0, :method_table => 1, :method_cache => 2, :name => 3, :constants => 4, :encloser => 5, :superclass => 6, :instance_fields => 7, :has_ivars => 8, :needs_cleanup => 9, :object_type => 10, :attached_instance => 11
  def __ivars__        ; @__ivars__         ; end
  # Do not define #methods as an accessor for @methods, use #method_table instead
  def method_cache     ; @method_cache      ; end
  def name             ; @name              ; end
  def constants_table  ; @constants         ; end
  def encloser         ; @encloser          ; end
  def superclass       ; @superclass        ; end
  def instance_fields  ; @instance_fields   ; end
  def instance_flags   ; @instance_flags    ; end
  def attached_instance; @attached_instance ; end

  def method_table
    @method_table
  end
end

