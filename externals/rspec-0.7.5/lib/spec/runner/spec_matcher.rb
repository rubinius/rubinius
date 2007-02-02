class SpecMatcher

  def initialize(context_and_or_spec_name, context_name)
    @context_name = context_name
    @name_to_match = context_and_or_spec_name
  end
  
  def matches?(spec_name)
    return true if matches_context? && (matches_spec?(spec_name) || context_only?)
    return true if matches_spec?(spec_name) && spec_only?(spec_name)
    return false
  end
  
  private
  
  def spec_only? spec
    @name_to_match == spec
  end
  
  def context_only?
    @name_to_match == @context_name
  end
  
  def matches_context?
    @name_to_match =~ /^#{Regexp.escape(@context_name)}\b/
  end
  
  def matches_spec?(spec_name)
    @name_to_match =~ /\b#{Regexp.escape(spec_name)}$/
  end
  
end
