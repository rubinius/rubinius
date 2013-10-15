# coding: utf-8

Gem::Specification.new do |spec|
  spec.name          = "rubinius-developer_tools"
  spec.version       = "2.0.0"
  spec.authors       = ["Brian Shirai"]
  spec.email         = ["brixen@gmail.com"]
  spec.description   = %q{A meta-gem for the Rubinius developer components like the debugger, profiler, coverage tool, etc.}
  spec.summary       = %q{A meta-gem for the Rubinius developer tools.}
  spec.homepage      = "http://rubini.us"
  spec.license       = "BSD"

  spec.add_dependency "rubinius-debugger", "~> 2.0"
  spec.add_dependency "rubinius-coverage", "~> 2.0"
  spec.add_dependency "rubinius-profiler", "~> 2.0"
end
