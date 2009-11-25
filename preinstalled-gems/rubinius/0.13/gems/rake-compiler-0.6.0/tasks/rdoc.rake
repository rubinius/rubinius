begin
  require 'rdoc/task'
rescue LoadError
  warn "RDoc 2.4.3+ gem is required, please install it (gem install rdoc)."
end

if defined?(RDoc) then
  DOC = RDoc::Task.new(:rdoc) do |rd|
    rd.title    = 'rake-compiler -- Documentation'
    rd.main     = 'README.rdoc'
    rd.rdoc_dir = 'doc/api'
    rd.options << '--line-numbers' << '--main'  << 'README.rdoc' << '--title' << 'rake-compiler -- Documentation'
    rd.rdoc_files.include %w(README.rdoc LICENSE.txt History.txt lib/**/*.rb)
  end
end
