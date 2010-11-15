# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{ruby-debug}
  s.version = "0.10.47"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Evan Phoenix"]
  s.date = %q{2010-11-15}
  s.description = %q{This gem is bundled with Rubinius to satisify any requirements on ruby-debug.
}
  s.email = %q{ephoenix@engineyard.com}
  s.files = ["LICENSE", "README", "Rakefile", "lib/ruby-debug.rb"]
  s.homepage = %q{http://rubini.us}
  s.require_paths = ["lib"]
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{Fast Ruby debugger - Rubinius Stub}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
