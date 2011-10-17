class Module
  def attr(*attributes)
    vis = Rubinius::VariableScope.of_sender.method_visibility
    attributes.each { |name| Rubinius.add_reader name, self, vis }

    return nil
  end
end
