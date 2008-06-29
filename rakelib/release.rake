# -*- ruby -*-

##
# Release Steps:
#
# 1) rake git:pull spec and make sure everything is clean.
# 2) if needed, update the version in rakelib/configuration.rb
#    commit, and rake git:safe_push.
# 3) rake release VERSION="X.Y.Z"
#    1) It packages up a tarball from HEAD on your master in cwd.
#    2) It tags HEAD on master with "vX.Y.Z", but is not pushed up.
# 4) double check the tarball - send out to osx, linux, and bsd for testing.
# 5) rake git:push
# 6) profit?
#

require 'rakelib/configuration'

desc "Perform a release"
task :release => %w(release:package release:tag)

desc "release tasks"
namespace :release do

  desc  "Generate CONTRIBUTORS file"
  task :contributors do
    sh "git log | grep Author | sort --unique | sed -e 's/Author: //' > CONTRIBUTORS"
  end

  desc "Print release steps"
  task :help do
    help = File.read(__FILE__).split(/\n\n+/)[1]

    puts help.gsub(/^\#+ ?/, '')
    puts
  end

  desc "Build a package from the git repo"
  task :package do
    ver = ENV['VERSION']
    abort "you need to specify VERSION=x.y.z" unless ver
    verify_version ver

    name = "rubinius-#{ver}"

    on_master do
      sh "git archive --format=tar --prefix=#{name}/ HEAD | gzip -9>#{name}.tgz"
    end
  end

  desc "Build a package from the git repo"
  task :tag do
    ver = ENV['VERSION']
    abort "you need to specify VERSION=x.y.z" unless ver
    verify_version ver

    name = "v#{ver}"

    on_master do
      # TODO: we _may_ want to switch from -a to -s for signed tarballs... maybe
      sh "git tag -a -m 'Tagging #{name} for release' #{name}"
      sh "git tag -l #{name}"
    end
  end
end

def verify_version expected
  actual = RBX_VERSION
  unless expected == actual
    abort "version #{expected} doesn't match #{actual}. Run `rake release:help`"
  end
end
