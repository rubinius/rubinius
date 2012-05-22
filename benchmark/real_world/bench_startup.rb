require 'benchmark'
require 'benchmark/ips'
require 'rbconfig'

Benchmark.ips(30) do |x|
  ruby = RbConfig::CONFIG["RUBY_INSTALL_NAME"] || RbConfig::CONFIG["ruby_install_name"]
  ruby << (RbConfig::CONFIG['EXEEXT'] || RbConfig::CONFIG['exeext'] || '')
  ruby = File.join(RbConfig::CONFIG['bindir'], ruby)

  x.report "startup" do |times|
    i = 0
    while i < times
      `#{ruby} -e ''`
      i += 1
    end
  end
end
