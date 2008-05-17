# rails currently needs this file and this one method.
module Test::Unit
  class Error
    def message
      "you're a loser"
    end
  end
end
