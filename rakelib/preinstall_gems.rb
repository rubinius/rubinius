require 'rubygems'
require 'rubygems/dependency_installer'
require 'rubinius/build_config'

puts "Pre-installing gems for #{RUBY_VERSION}..."

gems = Rubinius::BUILD_CONFIG[:runtime_gems]
options = {
  :wrappers             => true,
  :domain               => :local,
  :generate_ri          => false,
  :generate_rdoc        => false,
  :ignore_dependencies  => true,
  :conservative         => true
}

gems.each do |name, version|
  name_re = /#{name}/
  version_req = Gem::Requirement.create "~> #{version}"
  unless Gem::Specification.any? { |s| s.name =~ name_re and version_req =~ s.version }
    options[:args]        = name
    options[:version]     = version_req
    options[:env_shebang] = true
    inst = Gem::DependencyInstaller.new options
    s = inst.install(name, version_req).first
    puts "Installed #{s.name} #{s.version}"
  end
end
