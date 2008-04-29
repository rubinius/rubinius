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
#   action:  :add, :del
#   outcome: :pass, :fail, :all
#   tag:     the tag to create/delete
#   comment: the comment to create
#   tags:    zero or more tags to get matching
#            spec description strings from
#   desc:    zero or more strings to match the
#            spec description strings

class TagAction < ActionFilter
  def initialize(action, outcome, tag, comment, tags=nil, descs=nil)
    super tags, descs
    @action = action
    @outcome = outcome
    @tag = tag
    @comment = comment
    @report = []
  end

  def ===(string)
    return true unless @sfilter or @tfilter
    @sfilter === string or @tfilter === string
  end

  def after(state)
    if self === state.description and outcome? state
      tag = SpecTag.new
      tag.tag = @tag
      tag.comment = @comment
      tag.description = state.description

      case @action
      when :add
        changed = MSpec.write_tag tag
      when :del
        changed = MSpec.delete_tag tag
      end

      @report << state.description if changed
    end
  end

  def outcome?(state)
    @outcome == :all or
        (@outcome == :pass and not state.exception?) or
        (@outcome == :fail and state.exception?)
  end

  def report
    @report.join("\n") + "\n"
  end
  private :report

  def finish
    case @action
    when :add
      if @report.empty?
        print "\nTagAction: no specs were tagged with '#{@tag}'\n"
      else
        print "\nTagAction: specs tagged with '#{@tag}':\n\n"
        print report
      end
    when :del
      if @report.empty?
        print "\nTagAction: no tags '#{@tag}' were deleted\n"
      else
        print "\nTagAction: tag '#{@tag}' deleted for specs:\n\n"
        print report
      end
    end
  end

  def register
    super
    MSpec.register :after, self
    MSpec.register :finish, self
  end

  def unregister
    super
    MSpec.unregister :after, self
  end
end
