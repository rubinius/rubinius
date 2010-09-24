class BeValidDNSName
  # http://stackoverflow.com/questions/106179/regular-expression-to-match-hostname-or-ip-address
  # ftp://ftp.rfc-editor.org/in-notes/rfc3696.txt
  VALID_DNS = /^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])\.?$/

  def matches?(actual)
    @actual = actual
    (VALID_DNS =~ @actual) == 0
  end

  def failure_message
    ["Expected '#{@actual}'", "to be a valid DNS name"]
  end

  def negative_failure_message
    ["Expected '#{@actual}'", "not to be a valid DNS name"]
  end
end

class Object
  def be_valid_DNS_name
    BeValidDNSName.new
  end
end
