# -*- encoding: us-ascii -*-

class Symbol

  # Use equal? for ===
  alias_method :===, :equal?

  def self.all_symbols
    Rubinius.primitive :symbol_all_symbols
    raise PrimitiveFailure, "Symbol.all_symbols primitive failed."
  end
end
