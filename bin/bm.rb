#!/usr/bin/env rbx

TIMEOUT=10

Dir['./benchmark/**/bm*.rb'].each do |f|
  puts "Running #{f}..."
  system "./benchmark/timeout -t #{TIMEOUT} ./shotgun/rubinius #{f}; if [ $? -eq 143 ]; then echo '#{f} did not complete in #{TIMEOUT} seconds'; fi"
end
