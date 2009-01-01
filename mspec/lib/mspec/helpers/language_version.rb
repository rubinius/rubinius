require 'mspec/guards/guard'

class Object
  # Helper for syntax-sensitive specs. The specs should be placed in a file in
  # the +versions+ subdirectory. For example, suppose language/method_spec.rb
  # contains specs whose syntax depends on the Ruby version. In the
  # language/method_spec.rb use the helper as follows:
  #
  #   language_version __FILE__, "method"
  #
  # Then add a file "language/versions/method_1.8.rb" for the specs that are
  # syntax-compatible with Ruby 1.8.x.
  def language_version(dir, name)
    path = File.dirname(File.expand_path(dir))
    name = "#{name}_#{SpecGuard.ruby_version}.rb"
    file = File.join path, "versions", name

    require file if File.exists? file
  end
end
