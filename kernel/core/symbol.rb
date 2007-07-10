class Symbol
  def to_int
    warn "treating Symbol as an integer"
    self.to_i
  end
end