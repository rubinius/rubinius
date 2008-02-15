# depends on: module.rb

##
# Retrieves files from a .rba file.

module Archive
  # Allows giving non-compiled file names
  def self.get_object_fuzzy(path, name, version)
    case name
      when /\.rb$/
        name + 'c'
      when /\.rbc$/
        name
      else
        name + '.rbc'
    end

    get_object path, name, version
  end
end
