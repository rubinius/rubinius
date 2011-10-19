class Parser19Action
  def register
    MSpec.register :load,   self
    MSpec.register :unload, self
  end

  def load
    SpecGuard.ruby_version_override = "1.9.2"
  end

  def unload
    SpecGuard.ruby_version_override = nil
  end
end
