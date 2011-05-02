##
# Mixin containing byte classification methods.
#--
# isspace, islower, ... are not in MRI core library.

module Rubinius::CType
  def self.isctrl(num)
    num == ?\n or num == ?\r or num == ?\t or num == ?\f or
    num == ?\v or num == ?\a or num == ?\e or num == ?\b
  end

  def self.isspace(num)
    num == ?\s or num == ?\n or num == ?\t or num == ?\r or num == ?\f or num == ?\v
  end

  def self.isupper(num)
    num >= ?A and num <= ?Z
  end

  def self.islower(num)
    num >= ?a and num <= ?z
  end

  def self.isdigit(num)
    num >= ?0 and num <= ?9
  end

  def self.isalnum(num)
    islower(num) or isupper(num) or isdigit(num)
  end

  def self.toupper!(num)
    num - ?\s
  end

  def self.toupper(num)
    islower(num) ? toupper!(num) : num
  end

  def self.tolower!(num)
    num + ?\s
  end

  def self.tolower(num)
    isupper(num) ? tolower!(num) : num
  end

end
