require 'rubygems'
require 'rubygems/dependency_installer'

puts "Pre-installing gems for #{RUBY_VERSION}..."

gems = Dir["preinstalled-gems/*.gem"]
options = {
  :wrappers             => true,
  :domain               => :local,
  :generate_ri          => false,
  :generate_rdoc        => false,
  :ignore_dependencies  => true,
  :conservative         => true
}

gems.each do |gem|
  parts = File.basename(gem, ".gem").split "-"
  gem_name = parts[0..-2].join "-"
  gem_version = parts[-1]

  name_re = /#{gem_name}/
  version_req = Gem::Requirement.create "=#{gem_version}"
  unless Gem::Specification.any? { |s| s.name =~ name_re and version_req =~ s.version }
    options[:args]        = gem
    options[:version]     = version_req
    options[:env_shebang] = true
    inst = Gem::DependencyInstaller.new options
    inst.install gem, version_req

    puts "Installed #{gem_name}-#{gem_version}"
  end
end
