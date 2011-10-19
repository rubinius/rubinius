##
# Mixin containing byte classification methods.
#--
# isspace, islower, ... are not in MRI core library.

module Rubinius::CType
  # The character literals (?x) are Fixnums in 1.8 and Strings in 1.9
  # so we use literal values instead so this is 1.8/1.9 compatible.

  # \n \r \t \f \v \a \b \e
  def self.isctrl(num)
    (num >= 7 and num <= 13) or num == 27
  end

  # ' ' \n \t \r \f \v
  def self.isspace(num)
    num == 32 or (num >= 9 and num <= 13)
  end

  def self.isupper(num)
    num >= 65 and num <= 90
  end

  def self.islower(num)
    num >= 97 and num <= 122
  end

  def self.isdigit(num)
    num >= 48 and num <= 57
  end

  def self.isalnum(num)
    islower(num) or isupper(num) or isdigit(num)
  end

  def self.toupper!(num)
    num - 32
  end

  def self.toupper(num)
    islower(num) ? toupper!(num) : num
  end

  def self.tolower!(num)
    num + 32
  end

  def self.tolower(num)
    isupper(num) ? tolower!(num) : num
  end
end
