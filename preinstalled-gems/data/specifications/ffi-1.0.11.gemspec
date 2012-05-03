# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ffi"
  s.version = "1.0.11"

  if s.respond_to? :required_rubygems_version=
    s.required_rubygems_version = Gem::Requirement.new(">= 0")
  end
  s.authors = ["Brian Ford"]
  s.date = "2011-08-23"
  s.description = "This gem is bundled with Rubinius to satisfy dependencies on ffi gem. Rubinius provides FFI natively."
  s.email = "bford@engineyard.com"
  s.files = ["LICENSE", "README", "Rakefile", "lib/ffi.rb"]
  s.homepage = "http://rubini.us"
  s.require_paths = ["lib"]
  s.rubygems_version = "1.5.2"
  s.summary = "Foreign Function Interface for Ruby - Rubinius Stub Gem"

  if s.respond_to? :specification_version
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  end
end
