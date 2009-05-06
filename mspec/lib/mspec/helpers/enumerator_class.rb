class Object

  # Returns the Enumerator class (either Enumerator or Enumerable::Enumerator)
  # depending of the version.
  #
  def enumerator_class
    SpecVersion.new(RUBY_VERSION) < "1.8.7" ? Enumerable::Enumerator : Enumerator
  end
end
