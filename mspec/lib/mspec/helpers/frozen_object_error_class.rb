class Object

  # Returns the class of the exception raised when attempting to modify
  # a frozen object: either TypeError or RuntimeError, depending on the version.
  #
  def frozen_object_error_class
    SpecVersion.new(RUBY_VERSION) < "1.9" ? TypeError : RuntimeError
  end
end
