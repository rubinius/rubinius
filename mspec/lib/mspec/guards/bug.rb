require 'mspec/guards/version'

class BugGuard < VersionGuard
  def initialize(bug, version)
    @bug = bug
    @version = to_v version
  end

  def to_v(str)
    major, minor, tiny, patch = str.split "."
    tiny = 99 unless tiny
    patch = 9999 unless patch
    ("1%02d%02d%02d%04d" % [major, minor, tiny, patch].map { |x| x.to_i }).to_i
  end

  def match?
    implementation?(:ruby, :ruby18, :ruby19) && ruby_version <= @version
  end
end

class Object
  def ruby_bug(bug="Please add a bug tracker number", version="0")
    g = BugGuard.new bug, version
    yield if g.yield? true
    g.unregister
  end
end
