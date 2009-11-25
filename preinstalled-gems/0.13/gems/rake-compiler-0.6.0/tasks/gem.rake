require 'rubygems/package_task'

GEM_SPEC = Gem::Specification.new do |s|
  # basic information
  s.name        = "rake-compiler"
  s.version     = "0.6.0"
  s.platform    = Gem::Platform::RUBY

  # description and details
  s.summary     = 'Rake-based Ruby C Extension task generator.'
  s.description = "Provide a standard and simplified way to build and package\nRuby C extensions using Rake as glue."

  # dependencies
  s.add_dependency  'rake', '>= 0.8.3', '< 0.9'

  # development dependencies
  #s.add_development_dependency 'rspec', '~> 1.1.9'
  #s.add_development_dependency 'rcov', '~> 0.8.1'
  #s.add_development_dependency 'cucumber', '~> 0.1.8'

  # components, files and paths
  s.files = FileList["features/**/*.{feature,rb}", "bin/rake-compiler",
                      "lib/**/*.rb", "spec/**/*.rb", "tasks/**/*.rake",
                      "Rakefile", "*.{rdoc,txt,yml}"]

  s.executables = ['rake-compiler']

  s.require_path = 'lib'

  # documentation
  s.has_rdoc = true
  s.rdoc_options << '--main'  << 'README.rdoc' << '--title' << 'rake-compiler -- Documentation'

  s.extra_rdoc_files = %w(README.rdoc LICENSE.txt History.txt)

  # project information
  s.homepage          = 'http://github.com/luislavena/rake-compiler'
  s.rubyforge_project = 'rake-compiler'
  s.licenses          = ['MIT']

  # author and contributors
  s.author      = 'Luis Lavena'
  s.email       = 'luislavena@gmail.com'
end

gem_package = Gem::PackageTask.new(GEM_SPEC) do |pkg|
  pkg.need_tar = false
  pkg.need_zip = false
end

file "#{GEM_SPEC.name}.gemspec" => ['Rakefile', 'tasks/gem.rake'] do |t|
  puts "Generating #{t.name}"
  File.open(t.name, 'w') { |f| f.puts GEM_SPEC.to_yaml }
end

desc "Generate or update the standalone gemspec file for the project"
task :gemspec => ["#{GEM_SPEC.name}.gemspec"]
