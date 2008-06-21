require 'rake/rdoctask'
require 'rakelib/configuration'

task :doc => %w[
  doc:vm
  doc:rdoc
]

namespace :doc do

  task :vm => 'doc:vm:html'

  namespace :vm do

    desc "Remove all generated HTML files under doc/vm"
    task :clean do
      Dir.glob('doc/vm/**/*.html').each do |html|
        rm_f html unless html =~ /\/?index.html$/
      end
    end

    desc "Generate HTML in doc/vm"
    file 'doc/vm/toc.html' => %w[shotgun/lib/instructions.rb] do
      rbx 'doc/vm/gen_op_code_html.rb'
    end

    rule '.html' => %w[.txt doc/vm/rdoc.rb] do |t|
      rbx 'doc/vm/rdoc.rb', t.source, t.name
    end

    task :html => %w[
      build
      doc/vm/toc.html
      doc/vm/concurrency.html
      doc/vm/intro.html
      doc/vm/method_dispatch.html
      doc/vm/rbc_files.html
      doc/vm/rubinius_vs_mri.html
      doc/vm/shotgun.html
      doc/vm/toc.html
      doc/vm/vm_interfaces.html
      doc/vm/ffi.html
    ]
  end

  desc 'Generate rdoc for kernel, lib and stdlib'
  Rake::RDocTask.new :rdoc do |rd|
    rd.main = 'README'
    rd.title = "Rubinius #{RBX_VERSION} Documentation"

    rd.rdoc_dir = 'doc/rdoc'

    rd.rdoc_files.include 'README'
    rd.rdoc_files.include 'README-DEVELOPERS'
    rd.rdoc_files.include 'CONTRIBUTORS'
    rd.rdoc_files.include 'THANKS'
    rd.rdoc_files.include 'LICENSE'

    rd.rdoc_files.include 'kernel/core/*.rb'
    rd.rdoc_files.include 'kernel/bootstrap/*.rb'
    rd.rdoc_files.include 'kernel/platform/*.rb'

    rd.rdoc_files.include 'lib/**/*.rb'
  end

end

