# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{rake-compiler}
  s.version = "0.7.9"

  s.required_rubygems_version = Gem::Requirement.new(">= 1.3.5") if s.respond_to? :required_rubygems_version=
  s.authors = ["Luis Lavena"]
  s.date = %q{2011-06-08}
  s.default_executable = %q{rake-compiler}
  s.description = %q{Provide a standard and simplified way to build and package
Ruby extensions (C, Java) using Rake as glue.}
  s.email = %q{luislavena@gmail.com}
  s.executables = ["rake-compiler"]
  s.extra_rdoc_files = ["README.rdoc", "LICENSE.txt", "History.txt"]
  s.files = ["features/compile.feature", "features/cross-compile.feature", "features/cross-package-multi.feature", "features/cross-package.feature", "features/java-compile.feature", "features/java-no-native-compile.feature", "features/java-package.feature", "features/package.feature", "features/step_definitions/compilation.rb", "features/step_definitions/cross_compilation.rb", "features/step_definitions/execution.rb", "features/step_definitions/folders.rb", "features/step_definitions/gem.rb", "features/step_definitions/java_compilation.rb", "features/support/env.rb", "features/support/file_template_helpers.rb", "features/support/generator_helpers.rb", "features/support/platform_extension_helpers.rb", "bin/rake-compiler", "lib/rake/baseextensiontask.rb", "lib/rake/extensioncompiler.rb", "lib/rake/extensiontask.rb", "lib/rake/javaextensiontask.rb", "spec/spec.opts", "spec/lib/rake/extensiontask_spec.rb", "spec/lib/rake/javaextensiontask_spec.rb", "spec/spec_helper.rb", "spec/support/capture_output_helper.rb", "tasks/bin/cross-ruby.rake", "tasks/bootstrap.rake", "tasks/common.rake", "tasks/cucumber.rake", "tasks/gem.rake", "tasks/news.rake", "tasks/release.rake", "tasks/rspec.rake", "Rakefile", "Isolate", "README.rdoc", "History.txt", "LICENSE.txt", "cucumber.yml"]
  s.has_rdoc = nil
  s.homepage = %q{http://github.com/luislavena/rake-compiler}
  s.licenses = ["MIT"]
  s.rdoc_options = ["--main", "README.rdoc", "--title", "rake-compiler -- Documentation"]
  s.require_paths = ["lib"]
  s.required_ruby_version = Gem::Requirement.new(">= 1.8.6")
  s.rubyforge_project = %q{rake-compiler}
  s.rubygems_version = %q{1.3.6}
  s.summary = %q{Rake-based Ruby Extension (C, Java) task generator.}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<rake>, [">= 0"])
      s.add_development_dependency(%q<rspec>, ["~> 2.6.0"])
      s.add_development_dependency(%q<cucumber>, ["~> 0.10.6"])
    else
      s.add_dependency(%q<rake>, [">= 0"])
      s.add_dependency(%q<rspec>, ["~> 2.6.0"])
      s.add_dependency(%q<cucumber>, ["~> 0.10.6"])
    end
  else
    s.add_dependency(%q<rake>, [">= 0"])
    s.add_dependency(%q<rspec>, ["~> 2.6.0"])
    s.add_dependency(%q<cucumber>, ["~> 0.10.6"])
  end
end
