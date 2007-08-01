class MetaClass
  ivar_as_index :__ivars__ => 0, :methods => 1, :method_cache => 2, :name => 3, :constants => 4, :parent => 5, :superclass => 6, :instance_fields => 7, :instance_flags => 8, :attached_instance => 9
  def __ivars__        ; @__ivars__         ; end
  # Do not define #methods as an accessor for @methods, use #method_table instead
  def method_cache     ; @method_cache      ; end
  def name             ; @name              ; end
  def constants_table  ; @constants_table   ; end
  def parent           ; @parent            ; end
  def superclass       ; @superclass        ; end
  def instance_fields  ; @instance_fields   ; end
  def instance_flags   ; @instance_flags    ; end
  def attached_instance; @attached_instance ; end
  
  def method_table
    @methods
  end
end

