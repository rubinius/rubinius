Gem::Specification.new do |s|
  s.name                      = %q{mspec}
  s.version                   = "1.5.0"

  s.specification_version     = 2 if s.respond_to? :specification_version=

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors                   = ["Brian Ford"]
  s.date                      = %q{2008-05-21}
  s.email                     = %q{bford@engineyard.com}
  s.has_rdoc                  = true
  s.extra_rdoc_files          = %w[ README LICENSE ]
  s.executables               = ["mkspec", "mspec", "mspec-ci", "mspec-run", "mspec-tag"]
  s.files                     = FileList[ '{bin,lib,spec}/**/*.{yaml,txt,rb}', 'Rakefile', *s.extra_rdoc_files ]
  s.homepage                  = %q{http://rubyspec.org}
  s.rubyforge_project         = 'http://rubyforge.org/projects/mspec'
  s.require_paths             = ["lib"]
  s.rubygems_version          = %q{1.1.1}
  s.summary                   = <<EOS
MSpec is a specialized framework that is syntax-compatible
with RSpec for basic things like describe, it blocks and
before, after actions.

MSpec contains additional features that assist in writing
the RubySpecs used by multiple Ruby implementations. Also,
MSpec attempts to use the simplest Ruby language features
so that beginning Ruby implementations can run it.
EOS

  s.rdoc_options << '--title' << 'MSpec Gem' <<
                   '--main' << 'README' <<
                   '--line-numbers'
end