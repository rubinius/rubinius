##
# Mixin containing byte classification methods.
#--
# isspace, islower, ... are not in MRI core library.

module CType
  def isctrl
    self == ?\n or self == ?\r or self == ?\t or self == ?\f or
    self == ?\v or self == ?\a or self == ?\e or self == ?\b
  end

  def isspace
    self == ?\s or self == ?\n or self == ?\t or self == ?\r or self == ?\f or self == ?\v
  end

  def isupper
    self >= ?A and self <= ?Z
  end

  def islower
    self >= ?a and self <= ?z
  end

  def isdigit
    self >= ?0 and self <= ?9
  end

  def isalnum
    islower or isupper or isdigit
  end

  def toupper!
    self - ?\s
  end

  def toupper
    self.islower ? self.toupper! : self
  end

  def tolower!
    self + ?\s
  end

  def tolower
    self.isupper ? self.tolower! : self
  end

end
