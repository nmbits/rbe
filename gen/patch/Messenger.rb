
c = RBe::Cpp::Class["Messenger"]
if c
  # x Target
  %w(Target
     LockTarget
     LockTargetWithTimeout).each do |s|
    c.functions(s).each{|f| f.set_option :custom, true }
  end

  # SendMessage (TODO)

  %w(SendMessage).each do |s|
    c.functions(s).each{|f| f.set_option :custom, true }
  end

  c.ctors.first.set_option(:custom, true)

end
