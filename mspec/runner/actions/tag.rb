require 'mspec/runner/actions/filter'

# TagAction - Write tagged spec description string to a
# tag file associated with each spec file.
#
# The action is triggered by specs whose descriptions
# match the filter created with 'tags' and/or 'desc'
#
# The action fires in the :after event, after the spec
# had been run. The action fires if the outcome of
# running the spec matches 'outcome'.
#
# The arguments are:
#
#   outcome: :pass, :fail, :all
#   tag:     the tag to create
#   comment: the comment to create
#   tags:    zero or more tags to get matching
#            spec description strings from
#   desc:    zero or more strings to match the
#            spec description strings

class TagAction < ActionFilter
  def initialize(outcome, tag, comment, tags, desc)
    super tags, desc
    @outcome = outcome
    @tag = tag
    @comment = comment
  end
  
  def after(state)
    if @filter === state.description and outcome? state
      tag = SpecTag.new
      tag.tag = @tag
      tag.comment = @comment
      tag.description = state.description
      MSpec.write_tag tag
    end
  end
  
  def outcome?(state)
    @outcome == :all or
        (@outcome == :pass and not state.exception?) or
        (@outcome == :fail and state.exception?)
  end
  
  def register
    MSpec.register :after, self
  end
  
  def unregister
    MSpec.unregister :after, self
  end
end
