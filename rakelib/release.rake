require 'rakelib/configuration'

namespace :release do
  desc "Create the tarball for the release"
  task :tar do
    stamp = Time.now.strftime("%Y%m%d")
    sh "git archive --format=tar --prefix=rubinius-#{RBX_VERSION}/ HEAD | gzip > rubinius-#{RBX_VERSION}-#{stamp}.tar.gz"
  end

  desc "Tag HEAD with the current release"
  task :tag do
    sh "git tag release-#{RBX_VERSION} HEAD"
  end
end
