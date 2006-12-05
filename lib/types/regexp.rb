module Rubinius
  module Regexp
    Fields = [:source, :data, :names] 
  end
  
  add_type :regexp, Regexp
  
  module RegexpData
    Super  = nil
    Fields = []
  end
  
  add_type :regexpdata, RegexpData
  
  module MatchData
    Fields = [:source, :regexp, :full, :region]
  end
  
  add_type :matchdata, MatchData
end