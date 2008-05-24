require 'mspec/guards/guard'

class VersionGuard < SpecGuard
  def initialize(version)
    case version
    when String
      @version = to_v version
    when Range
      a = to_v version.first
      b = to_v version.last
      @version = version.exclude_end? ? a...b : a..b
    end
  end

  def to_v(str)
    major, minor, tiny, patch = str.split "."
    ("1%02d%02d%02d%04d" % [major, minor, tiny, patch].map { |x| x.to_i }).to_i
  end

  def ruby_version
    to_v("#{RUBY_VERSION}.#{RUBY_PATCHLEVEL}")
  end

  def match?
    case @version
    when Integer
      ruby_version >= @version
    when Range
      @version.include? ruby_version
    end
  end
end

class Object
  def ruby_version_is(*args)
    g = VersionGuard.new(*args)
    yield if g.yield?
    g.unregister
  end
end
