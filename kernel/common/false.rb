class FalseClass
  alias_method :|, :^
  alias_method :inspect, :to_s
end

FALSE = false
