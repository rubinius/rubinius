require 'benchmark'
 
total = (ENV['TOTAL'] || 1_000).to_i
 
Benchmark.bmbm do |x|
  x.report 'empty' do
    total.times do end
  end
 
  x.report 'fnmatch' do
    total.times do
      File.fnmatch 'doc/vm/*.txt', 'README'
      File.fnmatch 'doc/vm/*.txt', 'concurrency.html'
      File.fnmatch 'doc/vm/*.txt', 'concurrency.txt'
      File.fnmatch 'doc/vm/*.txt', 'gen_op_code_html.rb'
      File.fnmatch 'doc/vm/*.txt', 'gen_section_html.rb'
      File.fnmatch 'doc/vm/*.txt', 'index.html'
      File.fnmatch 'doc/vm/*.txt', 'intro.html'
      File.fnmatch 'doc/vm/*.txt', 'intro.txt'
      File.fnmatch 'doc/vm/*.txt', 'method_dispatch.html'
      File.fnmatch 'doc/vm/*.txt', 'method_dispatch.txt'
      File.fnmatch 'doc/vm/*.txt', 'op_code_info.rb'
      File.fnmatch 'doc/vm/*.txt', 'op_codes'
      File.fnmatch 'doc/vm/*.txt', 'rbc_files.html'
      File.fnmatch 'doc/vm/*.txt', 'rbc_files.txt'
      File.fnmatch 'doc/vm/*.txt', 'rdoc.rb'
      File.fnmatch 'doc/vm/*.txt', 'rubinius_vs_mri.html'
      File.fnmatch 'doc/vm/*.txt', 'rubinius_vs_mri.txt'
      File.fnmatch 'doc/vm/*.txt', 'section_template.html.erb'
      File.fnmatch 'doc/vm/*.txt', 'shotgun.html'
      File.fnmatch 'doc/vm/*.txt', 'shotgun.txt'
      File.fnmatch 'doc/vm/*.txt', 'styles.css'
      File.fnmatch 'doc/vm/*.txt', 'toc.html'
      File.fnmatch 'doc/vm/*.txt', 'vm_interfaces.html'
      File.fnmatch 'doc/vm/*.txt', 'vm_interfaces.txt'
    end
  end
 
  x.report 'glob' do
    total.times do
      Dir['doc/vm/*.txt']
    end
  end
end
