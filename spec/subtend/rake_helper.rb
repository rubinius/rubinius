require 'rake'
require 'rake/testtask'

task :default => [:clean, "#{$ext_name}.bundle"]


task :clean do
  cd $cwd do
    rm_rf "#{$ext_name}.bundle"
  end
end

file "#{$ext_name}.bundle" do
  cd $cwd do
    sh "cc -dynamic -bundle -undefined suppress -flat_namespace -I ../../../shotgun/lib/subtend -g -o #{$ext_name}.bundle #{$ext_name}.c"
  end
end
