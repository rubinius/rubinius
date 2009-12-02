require 'rake/clean'

# common pattern cleanup
CLEAN.include('tmp')

# set default task
task :default => [:spec, :features]

# make packing depend on success of running specs and features
task :package => [:spec, :features]

# make the release re-generate the gemspec if required
task :release => [:gemspec]

# publish documentation when doing a release
task :release => [:publish]
