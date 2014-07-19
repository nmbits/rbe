
c = RBe::Cpp::Class["Invoker"]
if c
  c.module = true
  c.custom_free = true

  # TODO
  # %w(InvokeKind).each do |s|
  #   c.functions(s).each{|f| f.set_option :noimp, true }
  # end

  %w(InvokeKind SetMessage SetTarget SetHandlerForReply Target).each do |s|
    c.functions(s).each{|f| f.set_option :custom, true }
  end
end
