require 'mspec/guards/version'

class BugGuard < VersionGuard
  def initialize(bug, version)
    @bug = bug
    @version = SpecVersion.new version, true
  end

  def match?
    standard? && ruby_version <= @version
  end
end

class Object
  def ruby_bug(bug="Please add a bug tracker number", version="0")
    g = BugGuard.new bug, version
    yield if g.yield? true
    g.unregister
  end
end
