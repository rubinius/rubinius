class MetaClass
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

